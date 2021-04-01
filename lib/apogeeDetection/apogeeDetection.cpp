/*
 * apogeeDetection.cpp
 *
 *  Created on: Dec 31, 2020
 *      Author: Vamshi
 */

#include <apogeeDetection.h>

using namespace std;

ApogeeDetection::ApogeeDetection() {}
ApogeeDetection::ApogeeDetection(double deltaT, double altitudeVar, double accelVar, double initAlt, double initAcc, double mainChuteDeployLoc) {

		DeltaT = deltaT;
		int n = 3;
		int m = 2;

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
		*_R << altitudeVar , 0,
		 			0,     accelVar;

		_z = new VectorXd(m);

		_x = new VectorXd(n);
		*_x << initAlt, initAcc, 0;

		mainDeployLoc = mainChuteDeployLoc + initAlt;

		kalmanfilter = new Kalman(n, *_F, m, 1, *_H, MatrixXd::Zero(n,1), *_Q, *_R, *_x);
}

double mean(double * arr, uint8_t len) {
	double sum = 0;
	for (int i=0; i<len; i++) {
		sum += arr[i];
	}
	return sum / len;
}

void ApogeeDetection::filter(double altitude, double accel_z){
	kalmanfilter->predict(VectorXd::Zero(1));
	*_z << altitude, accel_z;
	kalmanfilter->update(*_z);
}

bool ApogeeDetection::onGround() {
	return _onGround;
}

bool ApogeeDetection::engineLit() {
	return _engineLit;
}

bool ApogeeDetection::engineOff() {
	return !_engineLit;
}

bool ApogeeDetection::drogueReleased() {
	return _drogueReleased;
}

bool ApogeeDetection::engineStarted(double altitude, double accel_z) {
	filter(altitude, accel_z);
	if (kalmanfilter->_x[0] > previousAltitude) {
		currConsecutiveIncreases++;
	} else {
		currConsecutiveIncreases = 0;
	}
	// AVERAGE ACCEL > 15

	memmove(previousAccel, previousAccel + 1, sizeof(double)*(numPreviousAccel - 1));
	previousAccel[0] = kalmanfilter->_x[2];
	previousAltitude = kalmanfilter->_x[0];

	if (currConsecutiveIncreases >= outlook && mean(previousAccel, numPreviousAccel) > 15) {
		_engineLit = false;
		return true;
	}
	return false;
}

bool ApogeeDetection::MeCo(double altitude, double accel_z) {
	filter(altitude, accel_z);
	if (kalmanfilter->_x[2] < previousAccel[0]) {
		currConsecutiveAccelDecreases++;
	} else {
		currConsecutiveAccelDecreases = 0;
	}
	memmove(previousAccel, previousAccel + 1, sizeof(double)*(numPreviousAccel - 1));
	previousAccel[0] = kalmanfilter->_x[2];
	if (currConsecutiveAccelDecreases >= outlook) {
		_engineLit = false;
		return true;
	}
	return false;
}

bool ApogeeDetection::atApogee(double altitude, double accel_z) {
	filter(altitude, accel_z);

	if (kalmanfilter->_x[0] < previousAltitude) {
		currConsecutiveDecreases++;
	} else {
		currConsecutiveDecreases = 0;
	}
	previousAltitude = kalmanfilter->_x[0];
	if (currConsecutiveDecreases >= outlook) {
		_drogueReleased = true;
		return true;
	}
	return false;
}

bool ApogeeDetection::atMainChuteDeployLoc(double altitude, double accel_z) {
	filter(altitude, accel_z);

	if (fabs(kalmanfilter->_x[0] - mainDeployLoc) < tolerance) {
		return true;
	}
	return false;
}

ApogeeDetection::~ApogeeDetection() {
}
