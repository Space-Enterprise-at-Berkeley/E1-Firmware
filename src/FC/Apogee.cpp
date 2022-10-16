#include "Apogee.h"

/*
Detection methods:
- barometer
- magnetometer
*/

namespace Apogee {

	const uint32_t updatePeriod = 10 * 1000;

	uint32_t apogeeTime = 0;
	uint8_t apogeesFound = 0;
	bool apogeeCheck = false;
	bool launchCheck = false;

	const int sampleSize = 25;
	float velocitySamples[sampleSize] = {0};
	int velocityIndex = 0;

	float previousAltitude = 0;
	float previousVelocity = 0;
	float barometerVelocity = 0;

	void init() {

	}

	void start() {
		launchCheck = 1;
	}

	uint32_t checkForApogee() {
		barometerVelocity = altitudeToVelocity(HAL::bmp388.getAltitude());
		velocitySamples[velocityIndex] = barometerVelocity;
		velocityIndex = (velocityIndex + 1) % sampleSize;

		float total = 0;
		for (int i = 0; i < sampleSize; i ++) {
			total += velocitySamples[i];
		}
		float average = total / sampleSize;

		if (average < 0 && apogeeCheck == 0 && launchCheck == 1 && HAL::bmp388.getAltitude() > 100) {
			apogeeCheck = true;
			apogeeTime = millis();
			apogeesFound ++;
		}
		else if (average > 0.01) { // 10 feet per second?
			apogeeCheck = false; // reset in case of false positive
		}

		Comms::Packet apogeePacket = {.id = 28};
		Comms::packetAddFloat(&apogeePacket, apogeeTime);
		Comms::packetAddUint8(&apogeePacket, apogeesFound);
		Comms::emitPacket(&apogeePacket);
		return updatePeriod;
	}

	float altitudeToVelocity(float altitude) {
		float velocity = (altitude - previousAltitude) / HAL::bmp388.getUpdatePeriod();
		previousAltitude = altitude;
		previousVelocity = velocity;
		return barometerVelocity;
	}
}