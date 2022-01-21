#include "HAL.h"

namespace HAL {

    MAX31855 tcAmp1;
    MAX31855 tcAmp2;
    MAX31855 tcAmp3;
    MAX31855 tcAmp4;

    void initHAL() {
        SPI.begin();

        tcAmp1.init(&SPI, 21);
        tcAmp2.init(&SPI, 20);
        tcAmp3.init(&SPI, 19);
        tcAmp4.init(&SPI, 18);
    }
};
