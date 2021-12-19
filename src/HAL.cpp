#include <HAL.h>

namespace HAL {
    ADS8167 adc1;
    ADS8167 adc2;
    
    // INA for power supply
    uint8_t powerAddrs[numSupplies] = {0x41, 0x42, 0x43};
    INA219* supplyMonitors[numSupplies];

    // Pins and INA for valves/solenoids
    uint8_t valveAddrs[numValves] = {0x40, 0x42, 0x44, 0x45, 0x43, 0x41};
    uint8_t valvePins[numValves] = {5,  3,  1,  0,  2,  4, 6, 39};
    INA219* valveMonitors[numValves];
    
    
    void initHAL() {
        // initialize ADC 1
        adc1.init(&SPI, 37, 26, 9);
        adc1.setAllInputsSeparate();
        adc1.enableOTFMode();
        // initialize ADC 2
        adc2.init(&SPI, 36, 27, 10);
        adc2.setAllInputsSeparate();
        adc2.enableOTFMode();
        // etc...

        // Initialize INA219
        // values taken from old code, hotfire/main 68c4e9e
        float rShunt = 0.010;
        float maxExpectedCurrent = 5;
        Wire.begin();
        for (int i=0; i<numSupplies; i++) {
            supplyMonitors[i] = new INA219();
            supplyMonitors[i]->begin(&Wire, powerAddrs[i]);
            supplyMonitors[i]->configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
            supplyMonitors[i]->calibrate(rShunt, maxExpectedCurrent);
        }
        // ina1.begin(&Wire, 0x41);
        // ina1.configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        // ina1.calibrate(rShunt, maxExpectedCurrent);
        // ina2.begin(&Wire, 0x42);
        // ina2.configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        // ina2.calibrate(rShunt, maxExpectedCurrent);
        // ina3.begin(&Wire, 0x43);
        // ina3.configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        // ina3.calibrate(rShunt, maxExpectedCurrent);

        Wire1.begin();
        float valveRShunt = 0.033;
        for(int i=0; i<numValves; i++) {
            valveMonitors[i] = new INA219();
            valveMonitors[i]->begin(&Wire1, valveAddrs[i]);
            valveMonitors[i]->configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
            valveMonitors[i]->calibrate(valveRShunt, maxExpectedCurrent);
        }
        
        return 0; // success
    }
};
