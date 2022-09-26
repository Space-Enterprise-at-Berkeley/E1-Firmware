#pragma once
#include <BNO055.h>
#include <Comms.h>

namespace IMU {
	uint32_t updatePeriod = (0.0125) * 1000;

	void init();
	uint32_t sampleIMU();
	imu::Vector<(uint8_t)3U> getAcceleration();
}