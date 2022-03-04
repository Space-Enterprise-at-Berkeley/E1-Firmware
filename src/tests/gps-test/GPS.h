// #include "HAL.h"
#include <Wire.h>

#include "SparkFun_u-blox_GNSS_Arduino_Library.h"


namespace GPS {
    extern SFE_UBLOX_GNSS myGNSS;
    
    void initGPS();
    uint32_t latLongSample();
    uint32_t auxSample();



}

