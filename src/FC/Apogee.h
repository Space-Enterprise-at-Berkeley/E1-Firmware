#pragma once

#include "Comms.h"
#include "Barometer.h"

namespace Apogee {
    void init(Task *disableCommsTask);

    void startAltitudeDetection();
    uint32_t disableCommsTask();
}
