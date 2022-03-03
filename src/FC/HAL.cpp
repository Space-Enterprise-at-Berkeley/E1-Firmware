#include "HAL.h"

namespace HAL {
    BMP388_DEV bmp388;

    void initHAL() {
        Wire.begin();
        Wire.setClock(100000);

        // barometer
        bmp388.begin(0x76);        
    }
};
