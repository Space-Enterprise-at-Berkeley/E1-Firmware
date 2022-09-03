#pragma once

#include <Common.h>

#include <MAX31855.h>
#include <HX711.h>

#include <Arduino.h>
#include <SPI.h>

namespace HAL {
    extern MAX31855 tcAmp1;
    extern MAX31855 tcAmp2;
    extern MAX31855 tcAmp3;
    extern MAX31855 tcAmp4;

    extern HX711 lcAmp3;
    extern HX711 lcAmp4;

    void initHAL();

    const uint8_t tcAmp1_pin = 25; //A11
    const uint8_t tcAmp2_pin = 15; //A1
    const uint8_t tcAmp3_pin = 16; //A2
    const uint8_t tcAmp4_pin = 17; //A3
    
};