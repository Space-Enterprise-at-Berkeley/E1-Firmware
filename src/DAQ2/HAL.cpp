#include "HAL.h"

namespace HAL {

    MAX31855 tcAmp1;
    MAX31855 tcAmp2;
    MAX31855 tcAmp3;
    MAX31855 tcAmp4;

    HX711 lcAmp1;
    HX711 lcAmp2;

    void initHAL() {
        SPI.begin();


        lcAmp1.init(25, 24);
        lcAmp2.init(17, 18);
    }
};
