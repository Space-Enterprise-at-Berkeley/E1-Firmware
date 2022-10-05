#pragma once

#include "BMP388_DEV.h"

#include <Comms.h>

namespace Barometer {
    uint32_t getUpdatePeriod();
    float getAltitude();
    void init();
    uint32_t sampleBarometer();
}