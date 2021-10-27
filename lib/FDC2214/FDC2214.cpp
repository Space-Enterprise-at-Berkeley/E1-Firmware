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
        return false;
    }

    // define the config registers
    Adafruit_I2CRegister config_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_CONFIG, 2);
    config_reg.write(0x1C81);

    Adafruit_I2CRegister muxconfig_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_MUX_CONFIG, 2);
    muxconfig_reg.write(0x0208);

    Adafruit_I2CRegister settlecount_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_SETTLECOUNT_CH0, 2);
    settlecount_reg.write(0x000A); // 10 settlecount

    Adafruit_I2CRegister rcount_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_RCOUNT_CH0, 2);
    rcount_reg.write(0xFFFF); // Max resolution/rcount

    Adafruit_I2CRegister clockdiv_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_CLOCK_DIVIDERS_CH0, 2);
    clockdiv_reg.write(0x1001);

    Adafruit_I2CRegister drive_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DRIVE_CH0, 2);
    drive_reg.write(0x8800);

    //MUXCONFIG HERE

    return true;
}

unsigned long FDC2214::readSensor(){
    
    Adafruit_I2CRegister msb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH0_MSB, 2);
    Adafruit_I2CRegister lsb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH0_LSB, 2);
 
    unsigned long reading = (uint32_t)(msb_reg.read() & FDC2214_DATA_CHx_MASK_DATA) << 16;
    reading |= lsb_reg.read();
    Serial.println(reading);
    return reading;
}

void FDC2214::readCapacitance(float *data){
    const float sensorL = 0.000018; // 18 μH
    //const float sensorC = 0.000000000033; // 33 pF
    const float sensorC = 0; // 33 pF
    const float fRef = 43355000; //43.355 MHz

    float fSens = readSensor() * fRef / pow(2, 28);

    data[0] = (1 / (sensorL * pow(2 * PI * fSens, 2))) - sensorC;
    data[1] = -1;
    data[0] = 420;
}