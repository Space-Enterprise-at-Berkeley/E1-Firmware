#include "IMU.h"

namespace IMU {
	BNO055 bno055;

	void init() {
		bno055.begin()
	};

	uint32_t sampleIMU() {
		auto a = bno055.getVector(BNO055::VECTOR_LINEARACCEL);
		auto q = bno055.getQuat();

		float accelX, accelY, accelZ;
		float quatX, quatY, quatZ, quatW;

		accelX = a.x(), accelY = a.y(), accelZ = a.z();
		quatX = q.x(), quatY = q.y(), quatZ = q.z(), quatW = q.w();

		Comms::Packet imuPacket = { .id = 1234, .len = 0};

		Comms::packetAddFloat(&imuPacket, accelX);
		Comms::packetAddFloat(&imuPacket, accelY);
		Comms::packetAddFloat(&imuPacket, accelZ);

		Comms::packetAddFloat(&imuPacket, quatX);
		Comms::packetAddFloat(&imuPacket, quatY);
		Comms::packetAddFloat(&imuPacket, quatZ);
		Comms::packetAddFloat(&imuPacket, quatW);

		Comms::emitPacket(&imuPacket);

		// update every 3 seconds
		uint32_t updatePeriod = 3 * 1000;
		return updatePeriod;
	};

}