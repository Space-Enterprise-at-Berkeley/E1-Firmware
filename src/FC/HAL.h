#pragma once

#include <Common.h>

#include <ADS8167.h>
#include <INA219.h>
#include <INA226.h>
#include <MCP9600.h>
#include <TCA6408A.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MuxChannel.h>

namespace HAL {    
    
    extern ADS8167 adc1;
    //extern ADS8167 adc2;

    const float battShuntR = 0.002;
    const float battCurrMax = 8.0;
    const float supplyShuntR = 0.01;
    const float supplyCurrMax = 4.0;
    // extern INA226 supplyBatt;
    // extern INA219 supply12v;
    extern INA219 supply8v;

    extern TCA6408A ioExpander;

    const float chanShuntR = 0.033;
    const float chanCurrMax = 4.0;

    const uint8_t chan0Pin = 5;
    const uint8_t chan1Pin = 4;
    const uint8_t chan2Pin = 3;
    const uint8_t chan3Pin = 2;
    const uint8_t chan4Pin = 1;
    const uint8_t chan5Pin = 0;

    // NOTE: these pins refer to the IO Expander
    const uint8_t chan6Pin = 0;
    const uint8_t chan7Pin = 1;
    const uint8_t chan8Pin = 2;
    const uint8_t chan9Pin = 3;
    const uint8_t chan10Pin = 4;
    const uint8_t chan11Pin = 5;

    // These dont work at the moment
    // extern MCP9600 tcAmp0;
    // extern MCP9600 tcAmp1;
    // extern MCP9600 tcAmp2;
    // extern MCP9600 tcAmp3;

    extern MuxChannel muxChan0;
    extern MuxChannel muxChan1;
    extern MuxChannel muxChan2;
    extern MuxChannel muxChan3;
    extern MuxChannel muxChan4;
    extern MuxChannel muxChan5;
    extern MuxChannel muxChan6;
    extern MuxChannel muxChan7;
    extern MuxChannel muxChan8;
    extern MuxChannel muxChan9;
    extern MuxChannel muxChan10;
    extern MuxChannel muxChan11;
    extern MuxChannel muxChan12;
    extern MuxChannel muxChan13;
    extern MuxChannel muxChan14;
    extern MuxChannel muxChan15;

    // extern TCA6408A ioExpander0;
    // extern TCA6408A ioExpander1;

    // void setWireClockLow();
    // void resetWireClock();
    void initHAL();
};