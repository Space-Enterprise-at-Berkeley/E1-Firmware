#include "Apogee.h"

/*
Detection methods:
- accelerometer
- GPS
- barometer
- simulated
- IMU data (all 9 axis) (?)
- magnetometer
*/

namespace Apogee {

	const uint32_t millis = 10;

	void init() {

	}

	uint32_t checkForApogee() {

		return millis;
	}
}