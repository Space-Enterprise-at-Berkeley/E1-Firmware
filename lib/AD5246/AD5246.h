/*
  AD5246.cpp - A C++ library to interface with the AD5246 digital potentiometer
  used for the load cell amp.
  Created by Claire Chen, Nov 03, 2021.
*/  

#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Arduino.h>
#include <Wire.h>

#ifndef __AD5246__
#define __AD5246__

/** Default AD5246 I2C addresses. */
#define AD5246_I2C_ADDR_0   0x2C
// #define AD5246_I2C_ADDR_1   

class AD5246 {

public:
    AD5246();
    boolean begin(uint8_t i2c_addr = AD5246_I2C_ADDR_0,
                    TwoWire *theWire = &Wire);

    void setResistance(int *resistance);

};

#endif
