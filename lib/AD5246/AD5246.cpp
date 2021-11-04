/*
  AD5246.cpp - A C++ library to interface with the AD5246 digital potentiometer
  used for the load cell amp.
  Created by Claire Chen, Nov 03, 2021.
*/  

#include <AD5246.h>

AD5246::AD5246() //{ _device_id = ; }

boolean AD5246::begin(uint8_t i2c_addr, TwoWire *theWire) {
    
    i2c_dev = new Adafruit_I2CDevice(i2c_addr, theWire);

    /* Try to instantiate the I2C device. */
    if (!i2c_dev->begin(false)) { // *dont scan!*
        return false;
    }

    Adafruit_I2CRegister id_reg = //Adafruit_I2CRegister();
    
}