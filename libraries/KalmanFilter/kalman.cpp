/*
 * pid.cpp
 *
 *  Created on: Oct 22, 2020
 *      Author: Vamshi
 */

#include "kalman.h"

using namespace std;

Kalman::Kalman( float max, float min, float Kp, float Ki, float Kd ):
	max_(max),
	min_(min),
	Kp_(Kp),
	Kd_(Kd),
	Ki_(Ki),
	lastError_(0),
	integral_(0),
	lastTime_(millis())
{
}

Kalman::~Kalman() {
}
