/*
 * pid.h
 *
 *  Created on: Oct 22, 2020
 *      Author: Vamshi
 */

#ifndef PID_H_
#define PID_H_

#include <Arduino.h>

using namespace std;

class PID
{
    public:
        // Kp -  proportional gain
        // Ki -  Integral gain
        // Kd -  derivative gain
        // dt -  loop interval time
        // max - maximum value of actuator variable
        // min - minimum value of actuator variable
        PID( float max, float min, float Kp, float Ki, float Kd );

        // Returns the actuator variable given a setpoint and current process value
        float calculate(float setpoint, float pv );
        void reset( float max, float min, float Kp, float Ki, float Kd);
        void reset();
        ~PID();

    private:
		float max_;
		float min_;
		float Kp_;
		float Kd_;
		float Ki_;
		float lastError_;
		float integral_;
		long lastTime_;
};

#endif /* PID_H_ */
