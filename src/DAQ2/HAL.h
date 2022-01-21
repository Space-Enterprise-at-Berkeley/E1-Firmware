#pragma once

#include <Common.h>

#include <MAX31855.h>

#include <Arduino.h>
#include <SPI.h>

namespace HAL {
    extern MAX31855 tcAmp1;
    extern MAX31855 tcAmp2;
    extern MAX31855 tcAmp3;
    extern MAX31855 tcAmp4;

    void initHAL();
    
};