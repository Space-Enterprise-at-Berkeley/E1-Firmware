#pragma once

#include <Common.h>

#include <BMP388_DEV.h>
#include <BNO055.h>
#include <NEOM9N.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

namespace HAL {
    extern BMP388_DEV bmp388; // barometer
    extern BNO055 bno055; // imu
    extern SFE_UBLOX_GNSS neom9n; // gps

    void initHAL();
};