#include "stdafx.h"
#include <iostream>
#include "CImg/CImg.h"
#include <cmath>
#include <string>
#include <vector>
#include <ctime>
#include <conio.h>


#define S 3
#define C_MAX 255

using namespace cimg_library;
using namespace std;


void init();

int n, m, p;//p-���������� �������� �� ������ ����, n-������ �������� ���������, m - ������ �������� ���������
int N;//�-�� �������� �� ������ ����
double **W, **Wt;//������� ������� ����. ����� �������-������� ����� � ����� �������-��������
double *_X;//������� �� �������� ����
double **X;//������� �� ������� ����
double *dX;//�������
double *Y;//������� �� ������� ����
double e, alfa;//e - ����������� ���������� ������,alfa -����������� ��������
int width;//������ ��������� �����������
int height;//������ ��������� �����������
int h;
int w;
CImg <unsigned char> image("devyshka.bmp");
int L;//������ ��������� �������

void createMatrix();
void createMatrixW();
void transpW();
void createX();
void clearYX();
void getMatrixLayer(int q);

int _tmain(int argc, _TCHAR* argv[])
{
	width = image.width();
	height = image.height();
	init();//���� ��������

	N = n * m * 3;
	L = (width / n) * (height / m);
	w = width / n;
	h = height / m;
	createX();
	createMatrix();

 
	//���������� ������� ����� ����� ������� � ������� ������ ��������� �� ���������� ������� 
	createMatrixW();
	//������� ����� ����� ������� � �������� �����, ���������� ��� ���������������� ������� W
	transpW();

	//������������ �������� ��������������� �� ������� �� L ��������� ������� ������ N.
	int iterations = 0;//���������� ��������
	Y = new double[p];
	_X = new double[N];
	dX = new double[N];
	double Eq;//��������� ������������������ ������
	double E; //��������� ������
	do {
		E = 0;
		for (int k = 0; k < L; k++){
			clearYX();
			double *Xk = X[k];
			for (int i = 0; i < p; i++) {
				for (int j = 0; j < N; j++) {
					Y[i] += Xk[j] * W[j][i];
				}
			}
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < p; j++) {
					_X[i] += Y[j] * Wt[j][i];
				}
			}
			for (int i = 0; i < N;i++) {
				dX[i] = _X[i] - X[k][i];
			}

			for (int j = 0; j < p; j++) {
				double temp = 0;
				for (int q = 0; q < N; q++) {
					temp += dX[q] * Wt[j][q];
				}
				for (int i = 0; i < N; i++) {
					W[i][j] -= alfa * Xk[i] * temp;
				}
			}
			for (int i = 0; i < p; i++) {
				for (int j = 0; j < N; j++) {
					Wt[i][j] -= alfa * Y[i] * dX[j];
				}
			}

		}


		for (int q = 0; q < L; q++){
			clearYX();
			getMatrixLayer(q);
			for (int i = 0; i < N; i++) {
				dX[i] = _X[i] - X[q][i];
			}
			
			Eq = 0;
			for (int i = 0; i < N; i++){
				Eq += dX[i] * dX[i];
			}
			E += Eq;
		}
		cout << "iteration = " << iterations << "; " << "E = " << E << endl;
		iterations++;

	} while (E > e);//��������� ���� �� ����������� �������� ������
	int l = 0;
	//�������� �����������
	CImg <float> result(width, height, 1, 3, 0);
	for (int q = 0; q < L; q++) {
		clearYX();
		getMatrixLayer(q);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				for (int k = 0; k < 3; k++) {
					int pixel = (int)(255 * ((_X[(n * i + j)*S + k] + 1) / 2));
					if (pixel > 255) {
						pixel = 255;
					}
					if (pixel < 0) {
						pixel = 0;
					}
					result(q % w * n + j, q / w * m + i, 0, k) = pixel;
				}
			}
		}
	}
	result.display();
	result.save_bmp("result.bmp");
	double Z = (N*L) / ((N + L)*p + 2);
	cout << "Z = " << Z << endl;
	cout << "iterations = " << iterations << endl;
	cout << "E = " << E << endl;

	_getch();
	return 0;
}

void init(){
	cout << "Enter small image height(n) \n";
	cin >> n;
	cout << "Enter small image width(m) \n";
	cin >> m;
	cout << "Enter number of second layer neurons(p) \n";
	cin >> p;
	cout << "Enter max error(e).\n";
	cin >> e;
	cout << "Enter learning rate (alfa).\n";
	cin >> alfa;
}

void createMatrix() {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			for (int i1 = 0; i1 < n; i1++) {
				for (int j1 = 0; j1 < m; j1++) {
					for (int k = 0; k < 3; k++) {
						X[h * i + j][(n * i1 + j1)*S + k] = (2.0 * image(j*m + j1, i*n + i1, 0, k) / 255) - 1;
					}
				}
			}
		}
	}
}

void getMatrixLayer(int q){
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < N; j++) {
			Y[i] += X[q][j] * W[j][i];
		}
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < p; j++) {
			_X[i] += Y[j] * Wt[j][i];
		}
	}
}

void createX(){
	X = new double*[L];
	for (int index = 0; index < L; index++){
		X[index] = new double[N];
	}
}

void createMatrixW(){
	W = new double*[N];
	srand(time(0));
	for (int i = 0; i < N; i++){
		W[i] = new double[p];
		for (int j = 0; j < p; j++){
			W[i][j] = (((double)rand() / RAND_MAX) * 2 - 1);
		}
	}
}

void clearYX(){
		for (int i = 0; i < p; i++) {
			Y[i] = 0;
		}
		for (int i = 0; i < N; i++) {
			_X[i] = 0;
		}
}

void transpW(){
	Wt = new double*[p];
	for (int i = 0; i < p; i++){
		Wt[i] = new double[N];
		for (int j = 0; j < N; j++){
			Wt[i][j] = W[j][i];
		}
	}
}