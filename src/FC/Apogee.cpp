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
	float altitudeFound = 0;
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

	void disable() {
		launchCheck = 0;
	}

	uint32_t checkForApogee() {
		altitudeFound = Barometer::getAltitude();
		barometerVelocity = altitudeToVelocity(altitudeFound);
		velocitySamples[velocityIndex] = barometerVelocity;
		velocityIndex = (velocityIndex + 1) % sampleSize;

		float total = 0;
		for (int i = 0; i < sampleSize; i ++) {
			total += velocitySamples[i];
		}
		float average = total / sampleSize;

		if (average < 0 && apogeeCheck == 0 && launchCheck == 1 &&  altitudeFound > 100) {
			apogeeCheck = true;
			apogeeTime = millis();
			apogeesFound ++;

			DEBUG("APOGEE DETECTED: Time: ");
			DEBUG(apogeeTime);
			DEBUG("	Altitude: ");
			DEBUG(altitudeFound);
			DEBUG("	Apogees found: ");
			DEBUG(apogeesFound);
			DEBUG("\n");
			DEBUG_FLUSH();
		}
		else if (average > 0.01) { // 10 feet per second?
			apogeeCheck = false; // reset in case of false positive
		}

		DEBUG("Apogee check? ");
		DEBUG(apogeeCheck);
		DEBUG("\n");
		DEBUG_FLUSH();

		Comms::Packet apogeePacket = {.id = 28};
		// Comms::packetAddUint8(&apogeePacket, apogeeCheck);
		Comms::packetAddUint32(&apogeePacket, apogeeTime);
		Comms::packetAddFloat(&apogeePacket, altitudeFound);
		// Comms::packetAddUint8(&apogeePacket, apogeesFound);
		Comms::emitPacket(&apogeePacket);
		Comms::emitPacket(&apogeePacket, &RADIO_SERIAL, "\r\n\n", 3);
		DEBUG("Apogee packet sent\n");
		return updatePeriod;
	}

	float altitudeToVelocity(float altitude) {
		float velocity = (altitude - previousAltitude) / Barometer::getUpdatePeriod();
		previousAltitude = altitude;
		previousVelocity = velocity;
		return barometerVelocity;
	}
}