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

	const uint32_t updatePeriod = 10;
	bool beginDetection = false;

	uint32_t lastReading;

	void init() {

	}

	uint32_t checkForApogee() {
		uint32_t currentReading = millis();
		uint32_t delta_t = currentReading - lastReading;
		lastReading = currentReading;
		Comms::Packet apogeePacket = {.id = 1111};

		Comms::emitPacket(&apogeePacket);
		return updatePeriod;
	}
}