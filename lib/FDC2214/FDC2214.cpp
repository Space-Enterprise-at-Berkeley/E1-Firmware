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

    // define the config registers
    Adafruit_I2CRegister config_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_CONFIG, 2, MSBFIRST);
    config_reg.write(0xD481);

    Adafruit_I2CRegister muxconfig_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_MUX_CONFIG, 2, MSBFIRST);
    muxconfig_reg.write(0x0208);

    Adafruit_I2CRegister settlecount_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_SETTLECOUNT_CH3, 2, MSBFIRST);
    settlecount_reg.write(0x0400); // 10 settlecount

    Adafruit_I2CRegister rcount_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_RCOUNT_CH3, 2, MSBFIRST);
    rcount_reg.write(0xFFFF); // Max resolution/rcount

    Adafruit_I2CRegister clockdiv_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_CLOCK_DIVIDERS_CH3, 2, MSBFIRST);
    clockdiv_reg.write(0x1001);

    Adafruit_I2CRegister drive_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DRIVE_CH3, 2, MSBFIRST);
    drive_reg.write(0x9800);

    return true;
}

unsigned long FDC2214::readSensor(){
    
    Adafruit_I2CRegister msb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH3_MSB, 2, MSBFIRST);
    Adafruit_I2CRegister lsb_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_DATA_CH3_LSB, 2, MSBFIRST);
    
    //Serial.println(msb_reg.read());
    //Serial.println(lsb_reg.read());

    unsigned long reading = (uint32_t)(msb_reg.read() & FDC2214_DATA_CHx_MASK_DATA) << 16;
    reading |= lsb_reg.read();
    //Serial.println(reading);
    return reading;
}

void FDC2214::readCapacitance(float *data){
    const double sensorL = 0.000010; // 10 μH
    //const float sensorC = 0.000000000033; // 33 pF
    const double sensorC = 0; // 33 pF
    const double fRef = 43355000; //43.355 MHz

    float fSens = readSensor() * fRef / pow(2, 28);
    double capVal = (1.0 / (sensorL * pow(2.0* PI * fSens, 2.0))) - sensorC;

    data[0] = capVal * pow(10, 12);
    data[1] = -1;
}