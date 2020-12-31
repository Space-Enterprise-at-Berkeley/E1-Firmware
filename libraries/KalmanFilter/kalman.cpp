/*
 * kalman.cpp
 *
 *  Created on: Dec 30, 2020
 *      Author: Vamshi
 */

#include "kalman.h"

using namespace std;

Kalman::Kalman(int stateDim, MatrixXd F, int measurementDim, int inputDim,
	MatrixXd H, MatrixXd B, MatrixXd Q, MatrixXd R) {
		_n = stateDim;
		_m = measurementDim;
		_l = inputDim;

		_F = F;
		_H = H;
		_B = B;
		_Q = Q;
		_R = R;

		_K = MatrixXd(_n, _m);
		_S = MatrixXd(_m, _m);
		_P = MatrixXd::Identity(_n, _n);
		_x = VectorXd::Zero(_n);
		_z = VectorXd(_m);
		_y = VectorXd(_m);
}

void Kalman::predict(VectorXd u) {
	_x = _F*_x + _B*u;
	_P = _F*_P*_F.transpose() + _Q;
}

void Kalman::update(VectorXd z) {
	_y = z - _H*_x;
	_S = _H*_P*_H.transpose() + _R;
	_K = _P*_H.transpose()*_S.inverse();
	_x = _x + _K*_y;
	_P = (MatrixXd::Identity(_n, _n) - _K*_H)*_P;
}

Kalman::~Kalman() {
}
