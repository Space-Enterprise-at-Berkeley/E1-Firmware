#include "HAL.h"

namespace HAL {
    BMP388_DEV bmp388;
    BNO055 bno055;

    void initHAL() {
        Wire.begin();
        Wire.setClock(400000);

        // barometer
        bmp388.begin(0x76);

        // imu
        bno055.begin();
    }
};
