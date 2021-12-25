#pragma once

#include <HAL.h>

#include <Arduino.h>

namespace Thermocouples {
    extern uint32_t tcUpdatePeriod;

    extern float engineTC1Value;
    extern float engineTC2Value;
    extern float engineTC3Value;
    extern float engineTC4Value;

    void initThermocouples();
    uint32_t tcSample();
};
