#pragma once

#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace Thermocouples {
    extern uint32_t tcUpdatePeriod;

    void initThermocouples();
    uint32_t tcSample();

    uint32_t tc0Sample();
    uint32_t tc1Sample();
    uint32_t tc2Sample();
    uint32_t tc3Sample();
};
