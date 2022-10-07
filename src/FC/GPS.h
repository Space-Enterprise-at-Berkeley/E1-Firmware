#pragma once

// #include <NEOM9N.h>
#include <Comms.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

namespace GPS {
    void initGPS();
    uint32_t sampleGPS();
}