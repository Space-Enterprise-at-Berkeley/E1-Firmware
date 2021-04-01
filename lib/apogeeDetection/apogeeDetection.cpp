/*
 * apogeeDetection.cpp
 *
 *  Created on: Dec 31, 2020
 *      Author: Vamshi
 */

#include <apogeeDetection.h>

using namespace std;

ApogeeDetection::ApogeeDetection() {}

void ApogeeDetection::init(double deltaT, double altitudeVar, double accelVar, double initAlt, double initAcc) {
		DeltaT = deltaT;
		int n = 3;
		int m = 2;
		altitude = initAlt;
		acc_z = initAcc;

		_F = new MatrixXd(n, n);
		*_F << 1, DeltaT, pow(DeltaT, 2) / 2,
					0, 1     , DeltaT,
					0, 0     , 1;

		_H = new MatrixXd(m, n);
		*_H << 1, 0, 0,
						0, 0, 1;

		_Q = new MatrixXd(n, n);
		*_Q << 0, 0, 0,
					0, 0, 0,
					0, 0, 1;

		_R = new MatrixXd(m, m);
		*_R << altitudeVar, 0,
		 			0,     accelVar;

		_z = new VectorXd(m);

		_x = new VectorXd(n);
		*_x << altitude, 0, acc_z;

		kalmanfilter = new Kalman(n, *_F, m, 1, *_H, MatrixXd::Zero(n,1), *_Q, *_R, *_x);
}

void ApogeeDetection::filter(double altitude, double accel_z){
	kalmanfilter->predict(VectorXd::Zero(1));
	*_z << altitude, accel_z;
	kalmanfilter->update(*_z);
}

bool ApogeeDetection::atApogee() {
	filter(altitude, acc_z);

	if (kalmanfilter->_x[0] < previousAltitude) {
		currConsecutiveDecreases++;
	} else {
		currConsecutiveDecreases = 0;
	}
	previousAltitude = kalmanfilter->_x[0];
	previousAcc_z = kalmanfilter->_x[2];
	if (currConsecutiveDecreases >= outlook) {
		return true;
	}
	return false;
}

void ApogeeDetection::updateAlt(float data) {
	altitude = data;
}

void ApogeeDetection::updateAcc(float data) {
	acc_z = data;
}

double ApogeeDetection::getAlt() {
	return ApogeeDetection::altitude;
}

bool ApogeeDetection::weAtMECOBro() {
	if(acc_z - previousAcc_z <= -5)
		return true;
	else {
		return false;
	}
}

ApogeeDetection::~ApogeeDetection() {
}
