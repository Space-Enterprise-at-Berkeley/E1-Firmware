#pragma once

#include "BMP388_DEV.h"

#include <Comms.h>

namespace Barometer {
    uint32_t updatePeriod = (0.0125) * 1000;
    void init();
    uint32_t sampleBarometer();
}