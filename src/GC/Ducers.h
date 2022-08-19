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

    extern float pressurantPTROC;
    extern float loxTankPTROC;
    extern float fuelTankPTROC;
    extern float loxInjectorPTROC;
    extern float fuelInjectorPTROC;
    extern float loxDomePTROC;
    extern float fuelDomePTROC; 
    
    void initDucers();
    float interpolate1000(uint16_t rawValue);
    float interpolate5000(uint16_t rawValue);
    uint32_t ptSample();
    uint32_t pressurantPTROCSample();
};
