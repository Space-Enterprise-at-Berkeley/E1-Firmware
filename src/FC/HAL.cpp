#include "HAL.h"

namespace HAL {
    ADS8167 adc1;

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

    MCP9600 tcAmp0;
    MCP9600 tcAmp1;
    MCP9600 tcAmp2;
    MCP9600 tcAmp3;

    // Sensors breakouts
    BMP388_DEV bmp388;
    BNO055 bno055(28);
    SFE_UBLOX_GNSS neom9n;

    void initHAL() {
        // initialize ADC 1
        analogReadResolution(12); 

        adc1.init(&SPI, 37, 25);
        adc1.setAllInputsSeparate();
        adc1.enableOTFMode();

        // Initialize I2C buses
        Wire.begin();
        Wire.setClock(100000);

        // barometer
        bmp388.begin(0x76); // TODO check address

        // imu
        bno055.begin();

        // gps
        if(!neom9n.begin(SPI, gpsCSPin, 2000000)) {
            DEBUG("GPS DIDN'T INIT");
            DEBUG("\n");
        } else {
            DEBUG("GPS INIT SUCCESSFUL");
            DEBUG("\n");
        }
        
        // RS-485
        pinMode(RS485SwitchPin, OUTPUT);
        RS485_SERIAL.begin(921600); // Serial for capfill

        RADIO_SERIAL.begin(250000);

        DEBUG("radio\n");

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
        tcAmp0.init(0x60, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        tcAmp1.init(0x62, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0); 
        tcAmp2.init(0x66, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
        tcAmp3.init(0x65, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);

        // DEBUG("tc\n");

        muxChan0.init(&valveMux, 0, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan1.init(&valveMux, 1, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan2.init(&valveMux, 2, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan3.init(&valveMux, 3, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan4.init(&valveMux, 4, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan5.init(&valveMux, 5, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan6.init(&valveMux, 6, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan7.init(&valveMux, 7, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor); // valve channel 1
        muxChan8.init(&valveMux, 8, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan9.init(&valveMux, 9, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan10.init(&valveMux, 10, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan11.init(&valveMux, 11, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor);
        muxChan12.init(&valveMux, 12, valveMuxCurrentScalingFactor, valveMuxContinuityScalingFactor); // valve channel 6
        muxChan13.init(&valveMux, 13, valveMuxCurrentScalingFactor, valveMuxCurrentScalingFactor);
        muxChan14.init(&valveMux, 14, valveMuxCurrentScalingFactor, valveMuxCurrentScalingFactor);
        muxChan15.init(&valveMux, 15, valveMuxCurrentScalingFactor, valveMuxCurrentScalingFactor);

        DEBUG("mux\n");
    }
};
