#pragma once

#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace Thermocouples {
    extern uint32_t tcUpdatePeriod;

    extern float TC0Value;
    extern float TC1Value;
    extern float TC2Value;
    extern float TC3Value;

    void initThermocouples();
    uint32_t tcSample();

    uint32_t tc0Sample();
    uint32_t tc1Sample();
    uint32_t tc2Sample();
    uint32_t tc3Sample();
};
