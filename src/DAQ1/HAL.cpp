#include "HAL.h"

namespace HAL {

    ADS8167 adc;

    MCP9600 tcAmp0;
    MCP9600 tcAmp1;
    MCP9600 tcAmp2;
    MCP9600 tcAmp3;

    HX711 lcAmp0;
    HX711 lcAmp1;
    HX711 lcAmp2;

    void initHAL() {
        adc.init(&SPI, 37, 14, 15);
        adc.setAllInputsSeparate();
        adc.enableOTFMode();

        Wire.begin();

        tcAmp0.init(0x60, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        tcAmp1.init(0x61, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        tcAmp2.init(0x62, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        tcAmp3.init(0x63, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);

        lcAmp0.init(39, 38);
        lcAmp1.init(9, 10);
        lcAmp2.init(0, 1);
    }
};
