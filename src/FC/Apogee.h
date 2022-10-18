#pragma once

#include "Comms.h"
#include "Barometer.h"
#include "HAL.h"

namespace Apogee {

	void init();
	void start();
	void disable();
	uint32_t checkForApogee();
	float altitudeToVelocity(float altitude);
}