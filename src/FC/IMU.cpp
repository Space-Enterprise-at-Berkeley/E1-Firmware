#include "IMU.h"

namespace IMU {
	BNO055 bno055;

	uint32_t updatePeriod = 100 * 1000;
	imu::Vector<(uint8_t)3U> accelerometerReading;
	Comms::Packet imuPacket = { .id = 4, .len = 0};

	void init() {
		// if(bno055.isFullyCalibrated())
		Wire.begin();
		bno055.begin();
	}

	uint32_t sampleIMU() {
		accelerometerReading = bno055.getVector(BNO055::VECTOR_LINEARACCEL);
		auto &a = accelerometerReading; // keep the a alias so the code is cleaner
		auto m = bno055.getVector(BNO055::VECTOR_MAGNETOMETER);
		auto q = bno055.getQuat();

		float data[10] = {q.w(), q.x(), q.y(), q.z(), a.x(), a.y(), a.z(),  m.x(), m.y(), m.z()};
		// Serial.println(a.x());

		imuPacket.len = 0;
		for(float value : data)
			Comms::packetAddFloat(&imuPacket, value);
		Comms::emitPacket(&imuPacket);
		uint8_t sys, gyro, acc, mag;
		bno055.getCalibration(&sys, &gyro, &acc, &mag);
		// Serial.printf("calib vals: sys %d, gyro %d, acc %d, mag %d\n", sys, gyro, acc, mag);
		// Serial.printf("ax: %1.1f, ay: %1.1f, az: %1.1f\n", a.x(), a.y(), a.x());

		// 80 hz
		return updatePeriod;
	};

	imu::Vector<(uint8_t)3U> getAcceleration() {
		return accelerometerReading;
	}

}