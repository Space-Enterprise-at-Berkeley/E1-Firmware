#include <HAL.h>

namespace HAL {
    ADS8167 adc1;
    ADS8167 adc2;
    INA219 ina1;
    INA219 ina2;
    INA219 ina3;

    uint32_t initHAL() {
        // initialize ADC 1
        // initialize ADC 2
        // etc...

        // Initialize INA219
        // values taken from old code, hotfire/main 68c4e9e
        float rShunt = 0.010;
        float maxExpectedCurrent = 5;
        Wire.begin();
        ina1.begin(&Wire, 0x41);
        ina1.configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        ina1.calibrate(rShunt, maxExpectedCurrent);
        ina2.begin(&Wire, 0x42);
        ina2.configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        ina2.calibrate(rShunt, maxExpectedCurrent);
        ina3.begin(&Wire, 0x43);
        ina3.configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        ina3.calibrate(rShunt, maxExpectedCurrent);
        
        return 0; // success
    }
};
