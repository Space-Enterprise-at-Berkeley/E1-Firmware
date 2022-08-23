#pragma once

#include <Common.h>
#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace Ducers {
    extern uint32_t ptUpdatePeriod;

    extern float pressurantPTValue;
    extern float loxTankPTValue;
    extern float fuelTankPTValue;
    extern float loxInjectorPTValue;
    extern float fuelInjectorPTValue;
    extern float loxDomePTValue;
    extern float fuelDomePTValue;
    
    void initDucers();
    float interpolate1000(uint16_t rawValue);
    float interpolate5000(uint16_t rawValue);
    uint32_t ptSample();
};
