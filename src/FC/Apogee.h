#pragma once

#include <Common.h>
#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace Apogee {
    extern uint32_t apogeeDetectionPeriod;

    extern float previousAltitude;
    extern float barometerSampleRate;
    extern float previousVelocity;
    extern float accelerationThreshold;
    extern float interpretedAcceleration;
    extern float barometerAcceleration;
    extern float barometerVelocity;
    extern float velocityThreshold;
    extern uint8_t apogee;
    extern float mainParachuteAltitude;
    extern uint8_t mainChuteDeploy;
    
    void initApogee();
    float altitudeToAcceleration(float altitude);
    uint32_t apogeeDetectionTask();
};
