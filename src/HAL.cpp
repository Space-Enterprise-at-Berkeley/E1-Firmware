#include <HAL.h>

namespace HAL {
    ADS8167 adc1;
    ADS8167 adc2;

    INA226 supplyBatt;
    INA219 supply12v;
    INA219 supply8v;

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
    INA219 chan11;
    INA219 chan12;
    INA219 chan13;
    INA219 chan14;

    MCP9600 tcAmp0;
    MCP9600 tcAmp1;
    MCP9600 tcAmp2;
    MCP9600 tcAmp3;

    // TCA6408A ioExpander0;
    // TCA6408A ioExpander1;

    void initChannel(INA219 *channel, uint8_t address) {
        channel->init(&Wire1, address, chanShuntR, chanCurrMax, INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_SHUNT_BUS_CONT);
    }

    void setWireClockLow() {
        #define CLOCK_STRETCH_TIMEOUT 15000
        IMXRT_LPI2C_t *port = &IMXRT_LPI2C1;
        port->MCCR0 = LPI2C_MCCR0_CLKHI(55) | LPI2C_MCCR0_CLKLO(59) |
            LPI2C_MCCR0_DATAVD(25) | LPI2C_MCCR0_SETHOLD(40);
        port->MCFGR1 = LPI2C_MCFGR1_PRESCALE(2);
        port->MCFGR2 = LPI2C_MCFGR2_FILTSDA(5) | LPI2C_MCFGR2_FILTSCL(5) |
            LPI2C_MCFGR2_BUSIDLE(3000); // idle timeout 250 us
        port->MCFGR3 = LPI2C_MCFGR3_PINLOW(CLOCK_STRETCH_TIMEOUT * 12 / 256 + 1);

        port->MCCR1 = port->MCCR0;
            port->MCFGR0 = 0;
            port->MFCR = LPI2C_MFCR_RXWATER(1) | LPI2C_MFCR_TXWATER(1);
            port->MCR = LPI2C_MCR_MEN;
    }

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
        setWireClockLow();
        Wire1.begin();
        Wire1.setClock(1000000);

        // Initialize INA219s
        supplyBatt.init(&Wire, 0x43, battShuntR, battCurrMax, INA226_AVERAGES_1, INA226_BUS_CONV_TIME_8244US, INA226_SHUNT_CONV_TIME_8244US, INA226_MODE_SHUNT_BUS_CONT);
        supply12v.init(&Wire, 0x44, supplyShuntR, supplyCurrMax, INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_SHUNT_BUS_CONT);
        supply8v.init(&Wire, 0x45, supplyShuntR, supplyCurrMax, INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S, INA219_MODE_SHUNT_BUS_CONT);

        // valves
        initChannel(&chan0, 0x40);
        initChannel(&chan1, 0x41);
        initChannel(&chan2, 0x42);
        initChannel(&chan3, 0x43);
        initChannel(&chan4, 0x44);
        initChannel(&chan5, 0x45);
        // heaters
        initChannel(&chan6, 0x49);
        initChannel(&chan7, 0x4A);
        initChannel(&chan8, 0x4B);
        initChannel(&chan9, 0x4C);
        initChannel(&chan10, 0x4D);
        initChannel(&chan11, 0x4E);
        // chute
        initChannel(&chan12, 0x48);
        initChannel(&chan13, 0x47);
        // spare
        initChannel(&chan14, 0x46);

        // channels
        pinMode(chan0Pin, OUTPUT);
        pinMode(chan1Pin, OUTPUT);
        pinMode(chan2Pin, OUTPUT);
        pinMode(chan3Pin, OUTPUT);
        pinMode(chan4Pin, OUTPUT);
        pinMode(chan5Pin, OUTPUT);

        // thermocouple amplifiers
        tcAmp0.init(0x60, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        tcAmp1.init(0x61, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        tcAmp2.init(0x62, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        tcAmp3.init(0x63, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
    }
};
