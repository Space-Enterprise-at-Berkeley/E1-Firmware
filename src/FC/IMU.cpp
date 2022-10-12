#include "IMU.h"

namespace IMU {
	BNO055 bno055;

	uint32_t updatePeriod = 10 * 1000;
	imu::Vector<(uint8_t)3U> accelerometerReading;

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

		float data[10] = { a.x(), a.y(), a.z(), q.x(), q.y(), q.z(), q.w(), m.x(), m.y(), m.z()};
		// Serial.println(a.x());


		Comms::Packet imuPacket = { .id = 4, .len = 0};
		for(float value : data)
			Comms::packetAddFloat(&imuPacket, value);
		Comms::emitPacket(&imuPacket);
		uint8_t sys, gyro, acc, mag;
		// bno055.getCalibration(&sys, &gyro, &acc, &mag);
		// Serial.printf("calib vals: sys %d, gyro %d, acc %d, mag %d\n", sys, gyro, acc, mag);
		// Serial.printf("ax: %1.1f, ay: %1.1f, az: %1.1f, qx: %1.1f, qy: %1.1f, qz: %1.1f, qw: %1.1f, mx: %1.1f, my: %1.1f, mz: %1.1f\n", data[0], data[1], data[2], data[3],
		// data[4], data[5], data[6], data[7], data[8], data[9]);

		// 80 hz
		return updatePeriod;
	};

	imu::Vector<(uint8_t)3U> getAcceleration() {
		return accelerometerReading;
	}

}