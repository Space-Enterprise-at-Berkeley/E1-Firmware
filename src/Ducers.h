#pragma once

#include <HAL.h>

#include <Arduino.h>

namespace Ducers {
    extern uint32_t ptUpdatePeriod;

    extern float loxTankPTValue;
    extern float propTankPTValue;
    extern float loxInjectorPTValue;
    extern float propInjectorPTValue;
    extern float loxDomePTValue;
    extern float propDomePTValue;
    
    void initDucers();
    uint32_t ptSample();
};
