#include "Arduino.h"

#include <Wire.h>
#include "ADS1219.h"

ADS1219::ADS1219(int drdy, uint8_t addr, TwoWire *wire) {
  data_ready = drdy;
  address = addr;
  config = 0x00;
  singleShot = true;
  localWire = wire;
}

long ADS1219::getData(uint8_t conf) {
  localWire->beginTransmission(address);
  localWire->write(CONFIG_REGISTER_ADDRESS);
  localWire->write(conf);
  localWire->endTransmission();

  localWire->beginTransmission(address);
  localWire->write(0x08);
  localWire->endTransmission();

  while(digitalRead(data_ready)==1);
  delay(1);
  localWire->beginTransmission(address);
  localWire->write(0x10);
  localWire->endTransmission();

  localWire->requestFrom((uint8_t)address,(uint8_t)3);
  long data32 = Wire.read();
  data32 <<= 8;
  data32 |= Wire.read();
  data32 <<= 8;
  data32 |= Wire.read();
  return (data32 << 8) >> 8;
}

void ADS1219::start(){
  localWire->beginTransmission(address);
  localWire->write(0x08);
  localWire->endTransmission();
}

void ADS1219::powerDown(){
  localWire->beginTransmission(address);
  localWire->write(0x02);
  localWire->endTransmission();
}

void ADS1219::calibrate(){
  uint64_t calibrate = 0;
  for (int i = 0; i <  10; i++){
    calibrate += readShorted();
  }
  calibration = uint32_t (calibrate / 10);
}

uint8_t ADS1219::readRegister(uint8_t reg){
  localWire->beginTransmission(address);
  localWire->write(reg);
  localWire->endTransmission();
  localWire->requestFrom((uint8_t)address,(uint8_t)1);
  return localWire->read();
}

void ADS1219::writeRegister(uint8_t data){
  localWire->beginTransmission(address);
  localWire->write(CONFIG_REGISTER_ADDRESS);
  localWire->write(data);
  localWire->endTransmission();
}

long ADS1219::readConversionResult(){
  localWire->beginTransmission(address);
  localWire->write(0x10);
  localWire->endTransmission();
  localWire->requestFrom((uint8_t)address, (uint8_t)3);
  long data32 = localWire->read();
  data32 <<= 8;
  data32 |= localWire->read();
  data32 <<= 8;
  data32 |= localWire->read();
  return ((data32 << 8) >> 8) - calibration;
}

void ADS1219::resetConfig(){
	writeRegister(0x00);
}

long ADS1219::readData(int channel){
	config &= MUX_MASK;
	switch (channel){
    case (0):
      config |= MUX_SINGLE_0;
      break;
    case (1):
      config |= MUX_SINGLE_1;
      break;
    case (2):
      config |= MUX_SINGLE_2;
      break;
    case (3):
      config |= MUX_SINGLE_3;
      break;
	default:
	  break;
  }
  writeRegister(config);
    if(singleShot){
	     start();
    }
	  while(digitalRead(data_ready)==1){
      // Serial.println("waiting");
      ;
    }
	  return readConversionResult();
}

long ADS1219::readDifferential_0_1(){
  config &= MUX_MASK;
  config |= MUX_DIFF_0_1;
  writeRegister(config);
  start();
  while(digitalRead(data_ready)==1);
  return readConversionResult();
}

long ADS1219::readDifferential_2_3(){
  config &= MUX_MASK;
  config |= MUX_DIFF_2_3;
  writeRegister(config);
  start();
  while(digitalRead(data_ready)==1);
  return readConversionResult();
}

long ADS1219::readDifferential_1_2(){
  config &= MUX_MASK;
  config |= MUX_DIFF_1_2;
  writeRegister(config);
  start();
  while(digitalRead(data_ready)==1);
  return readConversionResult();
}

long ADS1219::readShorted(){
  config &= MUX_MASK;
  config |= MUX_SHORTED;
  writeRegister(config);
  while(digitalRead(data_ready)==1);
  return readConversionResult();
}

void ADS1219::setGain(adsGain_t gain){
  config &= GAIN_MASK;
  config |= gain;
  writeRegister(config);
}

void ADS1219::setDataRate(int rate){
	config &= DATA_RATE_MASK;
	switch (rate){
    case (20):
      config |= DATA_RATE_20;
      break;
    case (90):
      config |= DATA_RATE_90;
      break;
    case (330):
      config |= DATA_RATE_330;
      break;
    case (1000):
      config |= DATA_RATE_1000;
      break;
	default:
	  break;
  }
  writeRegister(config);
}

void ADS1219::setConversionMode(adsMode_t mode){
  config &= MODE_MASK;
  config |= mode;
  writeRegister(config);
  if (mode == CONTINUOUS){
	  singleShot = false;
    start();
  } else {
	  singleShot = true;
  }
}

void ADS1219::setVoltageReference(adsRef_t vref){
  config &= VREF_MASK;
  config |= vref;
  writeRegister(config);
}
