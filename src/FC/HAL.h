#pragma once

#include <Common.h>

#include <BMP388_DEV.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

namespace HAL {
    extern BMP388_DEV bmp388;

    void initHAL();
};