/*
 * apogeeDetection.cpp
 *
 *  Created on: Dec 31, 2020
 *      Author: Vamshi
 */

#include <kalman.h>
#include "apogeeDetection.h"

using namespace std;

ApogeeDetection::ApogeeDetection(double deltaT, double altitudeVar, double accelVar) {
		DeltaT = deltaT;

		_F = new MatrixXd(3, 3);
		*_F << 1, DeltaT, pow(DeltaT, 2) / 2,
					0, 1     , DeltaT,
					0, 0     , 1;

		_H = new MatrixXd(2, 3);
		*_H << 1, 0, 0,
						0, 0, 1;

		_Q = new MatrixXd(3, 3);
		*_Q << 0, 0, 0,
					0, 0, 0,
					0, 0, 1;

		_R = new MatrixXd(2, 2);
		*_R << altitudeVar , 0,
		 			0,     accelVar;

		_z = new VectorXd(2);

		kalmanfilter = new Kalman(3, *_F, 2, 1, *_H, MatrixXd::Zero(3,1), *_Q, *_R);
}

void ApogeeDetection::filter(double altitude, double accel_z){
	kalmanfilter->predict(VectorXd::Zero(1));
	*_z << altitude, accel_z;
	kalmanfilter->update(*_z);
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
		return true;
	}
	return false;
}

ApogeeDetection::~ApogeeDetection() {
}
