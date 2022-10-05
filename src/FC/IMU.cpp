#include "IMU.h"

namespace IMU {
	BNO055 bno055;

	uint32_t updatePeriod = 10 * 1000;
	imu::Vector<(uint8_t)3U> accelerometerReading;

	void init() {
		// if(bno055.isFullyCalibrated())
		bno055.begin();
	}

	uint32_t sampleIMU() {
		accelerometerReading = bno055.getVector(BNO055::VECTOR_LINEARACCEL);
		auto &a = accelerometerReading; // keep the a alias so the code is cleaner
		auto m = bno055.getVector(BNO055::VECTOR_MAGNETOMETER);
		auto q = bno055.getQuat();

		float data[10] = { a.x(), a.y(), a.z(), q.x(), q.y(), q.z(), q.w(), m.x(), m.y(), m.z()};

		Comms::Packet imuPacket = { .id = 4, .len = 0};
		for(float value : data)
			Comms::packetAddFloat(&imuPacket, value);
		Comms::emitPacket(&imuPacket);

		// 80 hz
		return updatePeriod;
	};

	imu::Vector<(uint8_t)3U> getAcceleration() {
		return accelerometerReading;
	}

}