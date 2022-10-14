#pragma once

#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace LoadCells {
    const uint32_t samplePeriod = 12.5 * 1000; // 80 Hz sample rate

    extern float loadCell0Value;
    extern float loadCell1Value;
    extern float loadCell2Value;
    extern float loadCell3Value;

    void initLoadCells();

    uint32_t sampleLoadCells();
};
