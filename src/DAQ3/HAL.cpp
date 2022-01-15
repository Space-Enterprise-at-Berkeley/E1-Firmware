#include "HAL.h"

namespace HAL {

    FDC2214 capSensor;

    void initHAL() {
        Wire.begin();

        capSensor.init(&Wire, 0x2A);
    }
};
