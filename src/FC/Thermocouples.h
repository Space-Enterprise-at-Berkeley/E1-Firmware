#pragma once

#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace Thermocouples {
    extern uint32_t tcUpdatePeriod;

    extern float engineTC0Value;
    extern float engineTC1Value;
    extern float engineTC2Value;
    extern float engineTC3Value;

    void initThermocouples();
    uint32_t tcSample();

    uint32_t tc0Sample();
    uint32_t tc1Sample();
    uint32_t tc2Sample();
    uint32_t tc3Sample();
};
