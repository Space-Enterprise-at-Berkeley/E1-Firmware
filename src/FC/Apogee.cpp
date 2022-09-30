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

	const uint32_t updatePeriod = (0.0125) * 1000;
	bool beginDetection = false;

	float lastReading;

	bool accelerometerActive = 0;
	bool gpsActive = 0;
	bool barometerActive = 0;
	bool magnetometerActive = 0;

	uint8_t accelerometerCount = 0;
	uint8_t gpsCount = 0;
	uint8_t barometerCount = 0;
	uint8_t magnetometerCount = 0;

	float accelerometerLastTime = 0;
	float gpsLastTime = 0;
	float barometerLastTime = 0;
	float magnetometerLastTime = 0;

	float accelerometerCumulativeVelocity = 0;

	void init() {

	}

	uint32_t checkForApogee() {
		float currentReading = millis();
		float time_delta = currentReading - lastReading;
		lastReading = currentReading;

		// check axis
		accelerometerCumulativeVelocity += IMU::getAcceleration().y() * time_delta;
		// check sign
		if (accelerometerActive && accelerometerCumulativeVelocity < 0) {
			accelerometerCount ++;
			accelerometerActive = false;
		}
		else if (!accelerometerActive && accelerometerCumulativeVelocity >= 0) {
			accelerometerActive = true;
		}

		Comms::Packet apogeePacket = {.id = 1111};

		Comms::packetAddUint8(&apogeePacket, accelerometerCount);
		Comms::packetAddFloat(&apogeePacket, accelerometerLastTime);

		Comms::packetAddUint8(&apogeePacket, gpsCount);
		Comms::packetAddFloat(&apogeePacket, gpsLastTime);

		Comms::packetAddUint8(&apogeePacket, barometerCount);
		Comms::packetAddFloat(&apogeePacket, barometerLastTime);

		Comms::packetAddUint8(&apogeePacket, magnetometerCount);
		Comms::packetAddFloat(&apogeePacket, magnetometerLastTime);

		Comms::emitPacket(&apogeePacket);
		return updatePeriod;
	}
}