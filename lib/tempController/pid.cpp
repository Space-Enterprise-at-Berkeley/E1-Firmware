/*
 * pid.cpp
 *
 *  Created on: Oct 22, 2020
 *      Author: Vamshi
 */

#include "pid.h"

using namespace std;

PID::PID( float max, float min, float Kp, float Ki, float Kd ):
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

float PID::calculate( float setpoint, float pv ) {
	long timeNow = millis();

	float dt = (float)(timeNow - lastTime_);
  lastTime_ = timeNow;

    // Calculate error
    float error = setpoint - pv;
    // Proportional term
    float Pout = Kp_ * error;
    // Integral term
    integral_ += error * dt;
    float Iout = Ki_ * integral_;
    // Derivative term
    float derivative = (error - lastError_) / dt;
    float Dout = Kd_ * derivative;
    // Calculate total output
    float output = Pout + Iout + Dout;

    // Restrict to max/min
    if ( output > max_ )
        output = max_;
    else if ( output < min_ )
        output = min_;

    // Save error to previous error
    lastError_ = error;

    return output;
}

void PID::reset( float max, float min, float Kp, float Ki, float Kd) {
	max_ = max;
	min_ = min;
	Kp_ = Kp;
	Kd_ = Kd;
	Ki_ = Ki;
	lastTime_ = millis();
	lastError_ = 0;
	integral_ = 0;
}

void PID::reset() {
    lastTime_ = millis();
    lastError_ = 0;
    integral_ = 0;
}

PID::~PID() {
}
