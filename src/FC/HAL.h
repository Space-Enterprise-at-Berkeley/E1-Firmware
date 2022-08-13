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
    // const float supplyShuntR = 0.01;
    const float supplyShuntR = 0.033; // Shunt on Flight V3 is 0.033 ohms
    const float supplyCurrMax = 4.0;
    // extern INA226 supplyBatt;
    // extern INA219 supply12v;
    extern INA219 supply8v;

    extern TCA6408A ioExpander;

    const float chanShuntR = 0.02; // Orig. 0.033 but now 20 mOhm on v2
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

    // New Pin Mappings for Flight Stack
    // Flight v3 Channels
    const uint8_t ; 
    const uint8_t ;
    const uint8_t ;
    const uint8_t ;
    const uint8_t ;

    // E-1 Extension Channels
    const uint8_t valve1Pin = 7;
    const uint8_t valve2Pin = 24;
    const uint8_t valve3Pin = 2;
    const uint8_t valve4Pin = 23;
    const uint8_t valve5Pin = 9;
    const uint8_t valve6Pin = 14;
    const uint8_t hBridge1Pin1 = 0;
    const uint8_t hBridge1Pin2 = 1;
    const uint8_t hBridge2Pin1 = 15;
    const uint8_t hBridge2Pin2 = 36;
    const uint8_t hBridge3Pin1 = 21;
    const uint8_t hBridge3Pin2 = 22;
    


    // These dont work at the moment
    // extern MCP9600 tcAmp0;
    // extern MCP9600 tcAmp1;
    // extern MCP9600 tcAmp2;
    // extern MCP9600 tcAmp3;

    extern MuxChannel muxChan0; // Chute1
    extern MuxChannel muxChan1; // Chute2
    extern MuxChannel muxChan2; // Cam1/NA
    extern MuxChannel muxChan3; // Amp/NA
    extern MuxChannel muxChan4; // Cam2/Break1
    extern MuxChannel muxChan5; // Radio/Break2
    extern MuxChannel muxChan6; // NA/Break3
    extern MuxChannel muxChan7; // Valve1
    extern MuxChannel muxChan8; // Valve2
    extern MuxChannel muxChan9; // Valve3
    extern MuxChannel muxChan10; // Valve4
    extern MuxChannel muxChan11; // Valve5
    extern MuxChannel muxChan12; // Valve6
    extern MuxChannel muxChan13; // HBridge1
    extern MuxChannel muxChan14; // HBridge2
    extern MuxChannel muxChan15; // HBridge3

    // extern TCA6408A ioExpander0;
    // extern TCA6408A ioExpander1;

    // void setWireClockLow();
    // void resetWireClock();
    void initHAL();
};