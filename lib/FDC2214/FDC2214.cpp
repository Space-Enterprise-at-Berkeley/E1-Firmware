/*
  FDC2214.cpp - A c++ library to interface with the FDC2214 
  capacitance to digital converter
  Created by Justin Yang, Oct 23, 2021.
*/                   

#include "FDC2214.h"

FDC2214::FDC2214() { _device_id = 0x3055; }

boolean FDC2214::begin(uint8_t i2c_addr, TwoWire *theWire) {
    i2c_dev = new Adafruit_I2CDevice(i2c_addr, theWire);

    /* Try to instantiate the I2C device. */
    if (!i2c_dev->begin(false)) { // *dont scan!*
        return false;
    }

    Adafruit_I2CRegister id_reg =
        Adafruit_I2CRegister(i2c_dev, FDC2214_DEVICE_ID, 2);

    if ((id_reg.read() >> 8) != _device_id) {
        Serial.println("bad_id");
        return false;
    }

    // Set sensor configuration registers

    Adafruit_I2CRegister config_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_CONFIG, 2, MSBFIRST);
    // Active channel 3; Sleep disabled; Full sensor activiation current; Internal oscillator; Interrupt unused; High current drive disabled
    config_reg.write(0x1C81);

    Adafruit_I2CRegister muxconfig_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_MUX_CONFIG, 2, MSBFIRST);
    // Continuous conversion; 10MHz deglitch
    muxconfig_reg.write(0xD20D);

    Adafruit_I2CRegister settlecount_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_SETTLECOUNT_CH0, 2, MSBFIRST);
    Adafruit_I2CRegister settlecount_regch1 = Adafruit_I2CRegister(i2c_dev, FDC2214_SETTLECOUNT_CH1, 2, MSBFIRST);
    // 16 settlecount
    settlecount_regch0.write(0x0010); 
    settlecount_regch1.write(0x0010); 

    Adafruit_I2CRegister rcount_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_RCOUNT_CH0, 2, MSBFIRST);
    Adafruit_I2CRegister rcount_regch1 = Adafruit_I2CRegister(i2c_dev, FDC2214_RCOUNT_CH1, 2, MSBFIRST);
    // 8192 RCount
    rcount_regch0.write(0x0200);
    rcount_regch1.write(0x0200);

    Adafruit_I2CRegister clockdiv_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_CLOCK_DIVIDERS_CH0, 2, MSBFIRST);
    Adafruit_I2CRegister clockdiv_regch1 = Adafruit_I2CRegister(i2c_dev, FDC2214_CLOCK_DIVIDERS_CH1, 2, MSBFIRST);
    // Single-ended configuration; 1x Clock divider
    clockdiv_regch0.write(0x2001); 
    clockdiv_regch1.write(0x2001); 

    Adafruit_I2CRegister drive_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_DRIVE_CH0, 2, MSBFIRST);
    Adafruit_I2CRegister drive_regch1 = Adafruit_I2CRegister(i2c_dev, FDC2214_DRIVE_CH1, 2, MSBFIRST);
    // .264mA sensor drive current
    drive_regch0.write(0x9800);
    drive_regch1.write(0x9800);

    low_pass0 = 0;
    low_pass1 = 0;

    return true;
}

unsigned long FDC2214::readSensor(int channel){
    Adafruit_I2CRegister msb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH0_MSB, 2, MSBFIRST);
    Adafruit_I2CRegister lsb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH0_LSB, 2, MSBFIRST);
    
    switch(channel){
        case 0:
            msb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH0_MSB, 2, MSBFIRST);
            lsb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH0_LSB, 2, MSBFIRST);
            break;
        case 1:
            msb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH1_MSB, 2, MSBFIRST);
            lsb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH1_LSB, 2, MSBFIRST);
            break;
    }
    //Serial.println(msb_reg.read());
    //Serial.println(lsb_reg.read());
    unsigned long reading = (uint32_t)(msb_reg.read() & FDC2214_DATA_CHx_MASK_DATA) << 16;
    reading |= lsb_reg.read();
    return reading;
}



void FDC2214::readCapacitance(float *data){
    const double fixedL = 0.000010; // 10 μH
    const double paraC0 = .00000000001359; // 13.59 pF
    const double paraC1 = .00000000001469; // 14.69 pF
    const double fRef = 43355000; //43.355 MHz

    float fSens0 = readSensor(0) * fRef / pow(2, 28);
    // double capVal0 = (1.0 / (fixedL * pow(2.0* PI * fSens0, 2.0)));
    double capVal0 = (pow(1/((fSens0 * 2) * PI * sqrt(fixedL * paraC0)) - 1, 2) - 1) * paraC0;

    float fSens1 = readSensor(1) * fRef / pow(2, 28);
    // double capVal1 = (1.0 / (fixedL * pow(2.0* PI * fSens1, 2.0)));
    double capVal1 = (pow(1/((fSens1 * 2) * PI * sqrt(fixedL * paraC1)) - 1, 2) - 1) * paraC1;

    // Serial.println(capVal0 * pow(10, 12));
    // Serial.println(capVal1 * pow(10, 12));

    // data[0] = capVal0 * pow(10, 12);
    // data[1] = (fSens0 * 2);
    // data[2] = capVal1 * pow(10, 12);
    // data[3] = (fSens1 * 2);

    low_pass0 = low_pass_alpha * low_pass0 + (1 - low_pass_alpha) * capVal0 * pow(10, 12);
    low_pass1 = low_pass_alpha * low_pass1 + (1 - low_pass_alpha) * capVal1 * pow(10, 12);

    data[0] = low_pass0;
    data[1] = (fSens0 * 2);
    data[2] = low_pass1;
    data[3] = (fSens1 * 2);

    data[4] = -1;
}