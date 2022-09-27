#pragma once
#include <BNO055.h>
#include <Comms.h>

namespace IMU {
	void init();
	uint32_t sampleIMU();
	imu::Vector<(uint8_t)3U> getAcceleration();
}