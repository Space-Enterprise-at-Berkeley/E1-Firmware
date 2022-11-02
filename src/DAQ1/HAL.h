#pragma once

#include <Common.h>

#include <ADS8167.h>
#include <MCP9600.h>
#include <HX711.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

namespace HAL {
    extern ADS8167 adc;

    extern MCP9600 tcAmp0;
    extern MCP9600 tcAmp1;
    extern MCP9600 tcAmp2;
    extern MCP9600 tcAmp3;

    extern HX711 lcAmp0;
    extern HX711 lcAmp1;
    extern HX711 lcAmp2;

    void initHAL();
    
};