#include "IMU.h"

namespace IMU {
	BNO055 bno055;

	void init() {
		bno055.begin()
	};

	uint32_t sampleIMU() {
		auto a = bno055.getVector(BNO055::VECTOR_LINEARACCEL);
		auto q = bno055.getQuat();

		float data[7] = { a.x(), a.y(), a.z(), q.x(), q.y(), q.z(), q.w() };

		Comms::Packet imuPacket = { .id = 1234, .len = 0};
		for(short int i = 0; i < 7; i++)
			Comms::packetAddFloat(&imuPacket, data[i]);
		Comms::emitPacket(&imuPacket);

		// 80 hz
		uint32_t updatePeriod = 80 * 1000;
		return updatePeriod;
	};

}