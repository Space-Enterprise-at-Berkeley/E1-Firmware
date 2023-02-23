/*
  FDC2214.h - A c++ library to interface with the FDC2214 
  capacitance to digital converter
  Created by Justin Yang, Oct 23, 2021.
*/                   

#ifndef __FDC2214__
#define __FDC2214__

#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Arduino.h>
#include <Wire.h>

/** Default FDC2214 I2C address. */

#define FDC2214_I2C_ADDR_0   0x2A
#define FDC2214_I2C_ADDR_1   0x2B
// Address is 0x2A (default) or 0x2B (if ADDR is high)

//bitmasks
#define FDC2214_CH0_UNREADCONV 0x0008         //denotes unread CH0 reading in STATUS register
#define FDC2214_CH1_UNREADCONV 0x0004         //denotes unread CH1 reading in STATUS register
#define FDC2214_CH2_UNREADCONV 0x0002         //denotes unread CH2 reading in STATUS register
#define FDC2214_CH3_UNREADCONV 0x0001         //denotes unread CH3 reading in STATUS register


//registers
#define FDC2214_DEVICE_ID           		0x7F
#define FDC2214_MUX_CONFIG          		0x1B
#define FDC2214_CONFIG              		0x1A
#define FDC2214_RCOUNT_CH0          		0x08
#define FDC2214_RCOUNT_CH1          		0x09
#define FDC2214_RCOUNT_CH2          		0x0A
#define FDC2214_RCOUNT_CH3          		0x0B
#define FDC2214_OFFSET_CH0		          0x0C
#define FDC2214_OFFSET_CH1          		0x0D
#define FDC2214_OFFSET_CH2    			    0x0E
#define FDC2214_OFFSET_CH3         			0x0F
#define FDC2214_SETTLECOUNT_CH0     		0x10
#define FDC2214_SETTLECOUNT_CH1     		0x11
#define FDC2214_SETTLECOUNT_CH2     		0x12
#define FDC2214_SETTLECOUNT_CH3     		0x13
#define FDC2214_CLOCK_DIVIDERS_CH0  		0x14
#define FDC2214_CLOCK_DIVIDERS_CH1  		0x15
#define FDC2214_CLOCK_DIVIDERS_CH2  		0x16
#define FDC2214_CLOCK_DIVIDERS_CH3  		0x17
#define FDC2214_STATUS              		0x18
#define FDC2214_DATA_CH0_MSB	          0x00
#define FDC2214_DATA_CH0_LSB    		    0x01
#define FDC2214_DATA_CH1_MSB	          0x02
#define FDC2214_DATA_CH1_LSB    		    0x03
#define FDC2214_DATA_CH2_MSB	          0x04
#define FDC2214_DATA_CH2_LSB    		    0x05
#define FDC2214_DATA_CH3_MSB	          0x06
#define FDC2214_DATA_CH3_LSB    		    0x07
#define FDC2214_DRIVE_CH0           		0x1E
#define FDC2214_DRIVE_CH1           		0x1F
#define FDC2214_DRIVE_CH2           		0x20
#define FDC2214_DRIVE_CH3           		0x21

// mask for 28bit data to filter out flag bits
#define FDC2214_DATA_CHx_MASK_DATA         	0x0FFF  
#define FDC2214_DATA_CHx_MASK_ERRAW        	0x1000  
#define FDC2214_DATA_CHx_MASK_ERRWD        	0x2000  


class FDC2214 {
public:
    FDC2214();
    boolean begin(uint8_t i2c_addr = FDC2214_I2C_ADDR_0,
                    TwoWire *theWire = &Wire);
    unsigned long readSensor(int channel); 
    // samhitag3 modified readCapacitance to allow for channel input
    float readCapacitance(int Channel);
    // samhitag3 add correctedCapacitance
    float correctedCapacitance(float avgRef, float baseline);
    float readDiffCapacitance();
protected:
  uint8_t _device_id = 0;
private:
  Adafruit_I2CDevice *i2c_dev;
};

#endif