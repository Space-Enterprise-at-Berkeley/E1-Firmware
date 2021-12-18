#include <HAL.h>

namespace HAL {
    ADS8167 adc1;
    ADS8167 adc2;

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
    }
};
