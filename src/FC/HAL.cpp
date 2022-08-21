#include "HAL.h"

namespace HAL {
    ADS8167 adc1;
    INA219 supply8v;

    //Voltage and Current Sense Mux
    Mux valveMux = {.muxSelect1Pin = 3,
                    .muxSelect2Pin = 4,
                    .muxSelect3Pin = 5,
                    .muxSelect4Pin = 6,
                    .muxInput1Pin = 40,
                    .muxInput2Pin = 16};

    MuxChannel muxChan0; // Chute1
    MuxChannel muxChan1; // Chute2
    MuxChannel muxChan2; // Cam1/NA
    MuxChannel muxChan3; // Amp/NA
    MuxChannel muxChan4; // Cam2/Break1
    MuxChannel muxChan5; // Radio/Break2
    MuxChannel muxChan6; // NA/Break3
    MuxChannel muxChan7; // Valve1
    MuxChannel muxChan8; // Valve2
    MuxChannel muxChan9; // Valve3
    MuxChannel muxChan10; // Valve4
    MuxChannel muxChan11; // Valve5
    MuxChannel muxChan12; // Valve6
    MuxChannel muxChan13; // HBridge1
    MuxChannel muxChan14; // HBridge2
    MuxChannel muxChan15; // HBridge3

    // Not usable rn
    MCP9600 tcAmp0;
    MCP9600 tcAmp1;
    MCP9600 tcAmp2;
    // MCP9600 tcAmp3;

    void initChannel(INA219 *channel, uint8_t address) {
        channel->init(&Wire1, address, chanShuntR, chanCurrMax, INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_SHUNT_BUS_CONT);
    }

    // void setWireClockLow() {
    //     Wire.setClock(100000);
    //     IMXRT_LPI2C_t *port = &IMXRT_LPI2C1;
    //     port->MCR = 0;
    //     port->MCFGR1 = LPI2C_MCFGR1_PRESCALE(2);
    //     port->MCR = LPI2C_MCR_MEN;
    // }

    // void resetWireClock() {
    //     Wire.setClock(400000);
    // }

    void initHAL() {
        // initialize ADC 1
        // adc1.init(&SPI, 37, 26, 9); // adc.init(&SPI, 37, 25, 999);
        analogReadResolution(12); 

        adc1.init(&SPI, 37, 25, 999);
        adc1.setAllInputsSeparate();
        adc1.enableOTFMode();

        // Initialize I2C buses
        Wire.begin();
        Wire.setClock(100000);

        Wire1.begin();
        Wire1.setClock(1000000);

        // Initialize INA219s
        // Only INA219 on FCv3 is on 8V supply, address 0x44
        supply8v.init(&Wire, 0x44, supplyShuntR, supplyCurrMax, INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_SHUNT_BUS_CONT);

        // Flight v3 channels
        pinMode(chute1Pin, OUTPUT);
        pinMode(chute2Pin, OUTPUT);
        pinMode(camPin, OUTPUT);
        pinMode(amp1Pin, OUTPUT);
        pinMode(radio1Pin, OUTPUT);

        // E-1 Extension channels
        pinMode(valve1Pin, OUTPUT);
        pinMode(valve2Pin, OUTPUT);
        pinMode(valve3Pin, OUTPUT);
        pinMode(valve4Pin, OUTPUT);
        pinMode(valve5Pin, OUTPUT);
        pinMode(valve6Pin, OUTPUT);
        pinMode(hBridge1Pin1, OUTPUT);
        pinMode(hBridge1Pin2, OUTPUT);
        pinMode(hBridge2Pin1, OUTPUT);
        pinMode(hBridge2Pin2, OUTPUT);
        pinMode(hBridge3Pin1, OUTPUT);
        pinMode(hBridge3Pin2, OUTPUT);

        // thermocouple amplifiers
        // 08/02: not on e1 ext
        // tcAmp0.init(0x60, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        // tcAmp1.init(0x62, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0); 
        // tcAmp2.init(0x63, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        // tcAmp3.init(0x67, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);

        muxChan0.init(&valveMux, 0);
        muxChan1.init(&valveMux, 1);
        muxChan2.init(&valveMux, 2);
        muxChan3.init(&valveMux, 3);
        muxChan4.init(&valveMux, 4);
        muxChan5.init(&valveMux, 5);
        muxChan6.init(&valveMux, 6);
        muxChan7.init(&valveMux, 7, valveMuxScalingFactor); // valve channel 1
        muxChan8.init(&valveMux, 8, valveMuxScalingFactor);
        muxChan9.init(&valveMux, 9, valveMuxScalingFactor);
        muxChan10.init(&valveMux, 10, valveMuxScalingFactor);
        muxChan11.init(&valveMux, 11, valveMuxScalingFactor);
        muxChan12.init(&valveMux, 12, valveMuxScalingFactor); // valve channel 6
        muxChan13.init(&valveMux, 13, valveMuxScalingFactor);
        muxChan14.init(&valveMux, 14, valveMuxScalingFactor);
        muxChan15.init(&valveMux, 15, valveMuxScalingFactor);
    }
};
