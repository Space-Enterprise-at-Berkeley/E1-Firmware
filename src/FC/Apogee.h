#pragma once

#include <Common.h>
#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace Apogee {    
    void initApogee();

    void startApogeeDetection();
    float altitudeToVelocity(float altitude);
    uint32_t apogeeDetectionTask();

};
