#pragma once
#include <BNO055.h>
#include <Comms.h>

namespace IMU {
	extern updatePeriod;

	void init();
	uint32_t sampleIMU();
}