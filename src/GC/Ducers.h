#pragma once

#include <Common.h>
#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace Ducers {
    extern uint32_t ptUpdatePeriod;

    extern float rqdPTValue;
    extern float purgePTValue;
    extern float mainValvePTValue;
    
    void initDucers();
    float interpolate1000(uint16_t rawValue);
    float interpolate5000(uint16_t rawValue);
    uint32_t ptSample();
};
