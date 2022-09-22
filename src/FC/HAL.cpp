#include "HAL.h"

namespace HAL {
    BMP388_DEV bmp388;
    BNO055 bno055;
    SFE_UBLOX_GNSS neom9n;

    void initHAL() {
        //intialize pins
        pinMode(breakWire1Pin, INPUT);
        pinMode(breakWire2Pin, INPUT);

        // Initialize I2C buses
        Wire.begin();
        Wire.setClock(100000);

        Wire.setClock(400000);

        // barometer
        bmp388.begin(0x76);

        // imu
        bno055.begin();

        // gps
        if(!neom9n.begin()) {
            DEBUG("GPS DIDN'T INIT");
            DEBUG("\n");
        } else {
            DEBUG("GPS INIT SUCCESSFUL");
            DEBUG("\n");
        }
    }
};
