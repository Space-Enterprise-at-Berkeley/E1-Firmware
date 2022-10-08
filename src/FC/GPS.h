#pragma once

#include "HAL.h"
#include <Arduino.h>

#include <Comms.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <SPI.h>


namespace GPS {
    
    void initGPS();
    uint32_t latLongSample();
    uint32_t auxSample();

}
