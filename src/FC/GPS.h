#include <Comms.h>
#include <Wire.h>

#include "NEOM9N.h"


namespace GPS {
    extern SFE_UBLOX_GNSS myGNSS;
    
    void initGPS();
    uint32_t latLongSample();
    uint32_t auxSample();



}

