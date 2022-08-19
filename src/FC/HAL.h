#pragma once

#include <Common.h>

#include <ADS8167.h>
#include <INA219.h>
#include <MCP9600.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MuxChannel.h>

namespace HAL {    
    
    extern ADS8167 adc1;

    const float battShuntR = 0.002;
    const float battCurrMax = 8.0;
    const float supplyShuntR = 0.033; // Shunt on Flight V3 is 0.033 ohms. Originally 0.01
    const float supplyCurrMax = 4.0;
    extern INA219 supply8v;

    const float chanShuntR = 0.02; // Orig. 0.033 but now 20 mOhm on v2
    const float chanCurrMax = 4.0;

    // Pin Mappings for Flight Stack
    // Flight v3 Channels
    const uint8_t chute1Pin = 30; 
    const uint8_t chute2Pin = 31;
    const uint8_t camPin = 39;
    const uint8_t amp1Pin = 32; // not flying amp but keeping the name for ref to schematic
    const uint8_t radio1Pin = 8;

    // E-1 Extension Channels
    const uint8_t valve1Pin = 7; // LOX GEMS
    const uint8_t valve2Pin = 24; // FUEL GEMS
    const uint8_t valve3Pin = 2; 
    const uint8_t valve4Pin = 23; 
    const uint8_t valve5Pin = 9;
    const uint8_t valve6Pin = 14; 
    const uint8_t hBridge1Pin1 = 0; 
    const uint8_t hBridge1Pin2 = 1; 
    const uint8_t hBridge2Pin1 = 15; // PRESS FLOW 1
    const uint8_t hBridge2Pin2 = 36; // PRESS FLOW 2
    const uint8_t hBridge3Pin1 = 21; 
    const uint8_t hBridge3Pin2 = 22; 

    // These dont work at the moment
    extern MCP9600 tcAmp0;
    extern MCP9600 tcAmp1;
    extern MCP9600 tcAmp2;
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

    // void setWireClockLow();
    // void resetWireClock();
    void initHAL();
};