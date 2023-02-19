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
    // Active channel 0; Sleep disabled; Full sensor activiation current; Internal oscillator; Interrupt unused; High current drive disabled
    // samhitag3 edited config register
    config_reg.write(0x1601);
    //config_reg.write(0x1C81);

    Adafruit_I2CRegister muxconfig_reg = Adafruit_I2CRegister(i2c_dev, FDC2214_MUX_CONFIG, 2, MSBFIRST);
    // Continuous conversion; 10MHz deglitch
    // samhitag3 edited muxconfig register
    muxconfig_reg.write(0xC20D);

    Adafruit_I2CRegister settlecount_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_SETTLECOUNT_CH0, 2, MSBFIRST);
    // samhitag3 added settlecount for ch1
    Adafruit_I2CRegister settlecount_regch1 = Adafruit_I2CRegister(i2c_dev, FDC2214_SETTLECOUNT_CH1, 2, MSBFIRST);
    // 16 settlecount
    settlecount_regch0.write(0x0010); 
    // samhitag3 added settlecount register for ch1
    settlecount_regch1.write(0x0010); 

    Adafruit_I2CRegister rcount_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_RCOUNT_CH0, 2, MSBFIRST);
    // samhitag3 added rcount for ch1
    Adafruit_I2CRegister rcount_regch1 = Adafruit_I2CRegister(i2c_dev, FDC2214_RCOUNT_CH1, 2, MSBFIRST);
    // 8192 RCount
    rcount_regch0.write(0xA800);
    // samhitag3 added rcount register for ch1
    rcount_regch1.write(0xA800);

    Adafruit_I2CRegister clockdiv_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_CLOCK_DIVIDERS_CH0, 2, MSBFIRST);
    // samhitag3 added clockdiv for ch1
    Adafruit_I2CRegister clockdiv_regch1 = Adafruit_I2CRegister(i2c_dev, FDC2214_CLOCK_DIVIDERS_CH1, 2, MSBFIRST);
    // Single-ended configuration; 1x Clock divider for differential; 2x for single ended
    clockdiv_regch0.write(0x2001); 
    // samhitag3 added clockdiv register for ch1
    clockdiv_regch1.write(0x2001); 
    //clockdiv_regch0.write(0x1001); 

    Adafruit_I2CRegister drive_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_DRIVE_CH0, 2, MSBFIRST);
    // samhitag3 added drive for ch1
    Adafruit_I2CRegister drive_regch1 = Adafruit_I2CRegister(i2c_dev, FDC2214_DRIVE_CH1, 2, MSBFIRST);
    // Sensor drive current
    //drive_regch0.write(0xA800);
    drive_regch0.write(0xF800);
    // samhitag3 added drive register for ch1
    drive_regch1.write(0xF800);
    
    // samhitag3 added offset registers for both channels
    Adafruit_I2CRegister offset_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_OFFSET_CH0, 2, MSBFIRST);
    Adafruit_I2CRegister offset_regch1 = Adafruit_I2CRegister(i2c_dev, FDC2214_OFFSET_CH1, 2, MSBFIRST);
    offset_regch0.write(0x0000);
    offset_regch1.write(0x0000);

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

// samhitag3 added channel input
float FDC2214::readCapacitance(int channel){
    const double fixedL = 0.000010; // 10 μH
    #ifdef LOX
    const double diffC = .000000000038; 
    #elif FUEL
    const double diffC = .000000000038; 
    #endif

    const double fRef = 40000000; //40 MHz
    // samhitag3 changed readSensor input
    float fSens = readSensor(channel) * fRef / pow(2, 28);
    //float capVal0 = (1.0 / (fixedL * pow(2.0* PI * fSens0, 2.0))) * pow(10, 12);
    float capVal = (pow(1/((fSens * 2) * PI * sqrt(fixedL * diffC)) - 1, 2) - 1) * diffC;

    return capVal * pow(10, 12);
}

// samhitag3 added correctedCapacitance method
float FDC2214::correctedCapacitance(float avgCap){
    return readCapacitance(00) - readCapacitance(01);
}

float FDC2214::readDiffCapacitance(){
    const double fixedL = 0.000010; // 10 μH
    const double fRef = 40000000; //40 MHz

    Adafruit_I2CRegister clockdiv_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_CLOCK_DIVIDERS_CH0, 2, MSBFIRST);
    Adafruit_I2CRegister drive_regch0 = Adafruit_I2CRegister(i2c_dev, FDC2214_DRIVE_CH0, 2, MSBFIRST);
    // Set 1x clock divider for differential measurement
    float fSens = readSensor(0) * fRef / pow(2, 28);
    float capVal = (1.0 / (fixedL * pow(2.0 * PI * fSens, 2.0)));

    return capVal * pow(10, 12);
}