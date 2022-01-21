#include "Hal.h"

namespace HAL {

    INA226 supplyBatt;
    INA219 supply12v;

    INA219 chan0;
    INA219 chan1;
    INA219 chan2;
    INA219 chan3;
    INA219 chan4;
    INA219 chan5;
    INA219 chan6;
    INA219 chan7;
    INA219 chan8;
    INA219 chan9;
    INA219 chan10;

    void initChannel(INA219 *channel, TwoWire *wire, uint8_t address) {
        channel->init(wire, address, chanShuntR, chanCurrMax, INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_SHUNT_BUS_CONT);
    }

    void initHAL() {
        // Initialize I2C buses
        Wire.begin();
        Wire.setClock(1000000);
        Wire1.begin();
        Wire1.setClock(1000000);

        // Initialize INA219s
        supplyBatt.init(&Wire, 0x40, battShuntR, battCurrMax, INA226_AVERAGES_1, INA226_BUS_CONV_TIME_8244US, INA226_SHUNT_CONV_TIME_8244US, INA226_MODE_SHUNT_BUS_CONT);
        supply12v.init(&Wire, 0x41, supplyShuntR, supplyCurrMax, INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_SHUNT_BUS_CONT);

        // Heater monitors
        initChannel(&chan0, &Wire1, 0x40);
        initChannel(&chan1, &Wire1, 0x41);
        initChannel(&chan2, &Wire, 0x42);
        initChannel(&chan3, &Wire, 0x43);
        // Actuator monitors
        initChannel(&chan4, &Wire, 0x44);
        initChannel(&chan5, &Wire, 0x45);
        initChannel(&chan6, &Wire, 0x46);
        initChannel(&chan7, &Wire, 0x47);
        initChannel(&chan8, &Wire, 0x48);
        initChannel(&chan9, &Wire, 0x49);
        initChannel(&chan10, &Wire, 0x4A);

        // Actuator control
        pinMode(hBrg1Pin1, OUTPUT);
        pinMode(hBrg1Pin2, OUTPUT);
        pinMode(hBrg2Pin1, OUTPUT);
        pinMode(hBrg2Pin2, OUTPUT);
        pinMode(hBrg3Pin1, OUTPUT);
        pinMode(hBrg3Pin2, OUTPUT);
        pinMode(hBrg4Pin1, OUTPUT);
        pinMode(hBrg4Pin2, OUTPUT);
        pinMode(hBrg5Pin1, OUTPUT);
        pinMode(hBrg5Pin2, OUTPUT);
        pinMode(hBrg6Pin1, OUTPUT);
        pinMode(hBrg6Pin2, OUTPUT);
        pinMode(hBrg7Pin1, OUTPUT);
        pinMode(hBrg7Pin2, OUTPUT);

        // Heater control
        pinMode(ctl12vChan1, OUTPUT);
        pinMode(ctl12vChan2, OUTPUT);
        pinMode(ctl24vChan1, OUTPUT);
        pinMode(ctl24vChan2, OUTPUT);
    }
};
