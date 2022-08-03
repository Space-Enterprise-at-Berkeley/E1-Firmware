#include "HAL.h"

namespace HAL {
    ADS8167 adc1;
    ADS8167 adc2;

    INA226 supplyBatt;
    INA219 supply12v;
    INA219 supply8v;

    TCA6408A ioExpander;

    //Voltage and Current Sense Mux
    Mux valveMux = {.muxSelect1Pin = 3,
                    .muxSelect2Pin = 4,
                    .muxSelect3Pin = 5,
                    .muxSelect4Pin = 6,
                    .muxInput1Pin = 40,
                    .muxInput2Pin = 16};

    MuxChannel muxChan0;
    MuxChannel muxChan1;
    MuxChannel muxChan2;
    MuxChannel muxChan3;
    MuxChannel muxChan4;
    MuxChannel muxChan5;
    MuxChannel muxChan6;
    MuxChannel muxChan7;
    MuxChannel muxChan8;
    MuxChannel muxChan9;
    MuxChannel muxChan10;
    MuxChannel muxChan11;
    MuxChannel muxChan12;
    MuxChannel muxChan13;
    MuxChannel muxChan14;
    MuxChannel muxChan15;

    // Not usable rn
    // MCP9600 tcAmp0;
    // MCP9600 tcAmp1;
    // MCP9600 tcAmp2;
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
        adc1.init(&SPI, 37, 26, 9);
        adc1.setAllInputsSeparate();
        adc1.enableOTFMode();
        // initialize ADC 2
        adc2.init(&SPI, 36, 27, 10);
        adc2.setAllInputsSeparate();
        adc2.enableOTFMode();

        // Initialize I2C buses
        Wire.begin();
        Wire.setClock(100000);

        Wire1.begin();
        Wire1.setClock(1000000);

        // Initialize INA219s
        supplyBatt.init(&Wire, 0x43, battShuntR, battCurrMax, INA226_AVERAGES_1, INA226_BUS_CONV_TIME_8244US, INA226_SHUNT_CONV_TIME_8244US, INA226_MODE_SHUNT_BUS_CONT);
        supply12v.init(&Wire, 0x44, supplyShuntR, supplyCurrMax, INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_SHUNT_BUS_CONT);
        supply8v.init(&Wire, 0x45, supplyShuntR, supplyCurrMax, INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_SHUNT_BUS_CONT);

        // IO Expander
        ioExpander.init(0x20, &Wire);

        // todo: channels now don't have ina219's
        // valves
        // initChannel(&chan0, 0x40);
        // initChannel(&chan1, 0x41);
        // initChannel(&chan2, 0x42);
        // initChannel(&chan3, 0x43);
        // initChannel(&chan4, 0x44);
        // initChannel(&chan5, 0x45);
        // heaters
        // initChannel(&chan6, 0x49);
        // initChannel(&chan7, 0x4A);
        // initChannel(&chan8, 0x4B);
        // initChannel(&chan9, 0x4C);
        // initChannel(&chan10, 0x4D);
        // initChannel(&chan11, 0x4E);
        // chute
        // initChannel(&chan12, 0x48);
        // initChannel(&chan13, 0x47);
        // spare
        // initChannel(&chan14, 0x46);

        // channels
        pinMode(chan0Pin, OUTPUT);
        pinMode(chan1Pin, OUTPUT);
        pinMode(chan2Pin, OUTPUT);
        pinMode(chan3Pin, OUTPUT);
        pinMode(chan4Pin, OUTPUT);
        pinMode(chan5Pin, OUTPUT);

        // thermocouple amplifiers
        // 08/02: not on e1 ext
        // tcAmp0.init(0x60, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        // tcAmp1.init(0x61, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        // tcAmp2.init(0x62, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        // tcAmp3.init(0x63, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);

        muxChan0.init(&valveMux, 0);
        muxChan1.init(&valveMux, 1);
        muxChan2.init(&valveMux, 2);
        muxChan3.init(&valveMux, 3);
        muxChan4.init(&valveMux, 4);
        muxChan5.init(&valveMux, 5);
        muxChan6.init(&valveMux, 6);
        muxChan7.init(&valveMux, 7);
        muxChan8.init(&valveMux, 8);
        muxChan9.init(&valveMux, 9);
        muxChan10.init(&valveMux, 10);
        muxChan11.init(&valveMux, 11);
        muxChan12.init(&valveMux, 12);
        muxChan13.init(&valveMux, 13);
        muxChan14.init(&valveMux, 14);
        muxChan15.init(&valveMux, 15);
    }
};
