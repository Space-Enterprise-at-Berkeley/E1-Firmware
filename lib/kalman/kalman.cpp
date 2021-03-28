/*
 * kalman.cpp
 *
 *  Created on: Dec 30, 2020
 *      Author: Vamshi
 */

#include "kalman.h"

using namespace std;

Kalman::Kalman(int stateDim, MatrixXd F, int measurementDim, int inputDim,
	MatrixXd H, MatrixXd B, MatrixXd Q, MatrixXd R, VectorXd initX) {
		_n = stateDim;
		_m = measurementDim;
		_l = inputDim;

		_F = F;
		_H = H;
		__B = B;
		_Q = Q;
		_R = R;

		_K = MatrixXd(_n, _m);
		__S = MatrixXd(_m, _m);
		__P = MatrixXd::Identity(_n, _n);
		_x = initX;
		_y = VectorXd(_m);
}

void Kalman::predict(VectorXd u) {
	_x = _F*_x + __B*u;
	__P = _F*__P*_F.transpose() + _Q;
}

void Kalman::update(VectorXd z) {
	_y = z - _H*_x;
	__S = _H*__P*_H.transpose() + _R;
	_K = __P*_H.transpose()*__S.inverse();
	_x = _x + _K*_y;
	__P = (MatrixXd::Identity(_n, _n) - _K*_H)*__P;
}

Kalman::~Kalman() {
}
