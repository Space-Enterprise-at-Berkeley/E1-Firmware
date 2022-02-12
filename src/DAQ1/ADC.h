#pragma once

#include <Common.h>
#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace ADC {
    extern uint32_t adcUpdatePeriod;
    
    void initADC();
    uint32_t fastADCSample();
    uint32_t adcSample();
};
