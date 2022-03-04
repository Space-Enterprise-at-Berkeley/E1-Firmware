#include "HAL.h"

#include <NEOM9N.h>

#include <Common.h>
#include <Comms.h>
#include <Wire.h>


namespace GPS {
    
    void initGPS();
    uint32_t latLongSample();
    uint32_t auxSample();

}

