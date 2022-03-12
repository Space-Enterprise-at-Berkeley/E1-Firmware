#pragma once

#include "Arduino.h"
#include "Comms.h"
#include "BlackBox.h"
#include "HAL.h"

namespace BreakWire {

    const uint32_t samplingPeriod = 50 * 1000;
    const float scalingFactor = 3.3 / 4096;

    void init();
    uint32_t sampleBreakWires();
}

