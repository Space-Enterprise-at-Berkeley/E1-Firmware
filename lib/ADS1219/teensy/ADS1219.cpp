#include "Arduino.h"

#include <Wire.h>
#include "ADS1219.h"

ADS1219::ADS1219(uint8_t drdy, uint8_t addr, TwoWire *wire):
  ADC(drdy) {
  _rdy_pin = drdy;
  address = addr;
  config = 0x00;
  singleShot = true;
  _wire = wire;
  _wire->begin();
}

long ADS1219::getData(uint8_t conf) {
  _wire->beginTransmission(address);
  _wire->write(CONFIG_REGISTER_ADDRESS);
  _wire->write(conf);
  _wire->endTransmission();

  _wire->beginTransmission(address);
  _wire->write(0x08);
  _wire->endTransmission();

  while(digitalRead(_rdy_pin)==1);
  delay(1);
  _wire->beginTransmission(address);
  _wire->write(0x10);
  _wire->endTransmission();

  _wire->requestFrom((uint8_t)address,(uint8_t)3);
  long data32 = _wire->read();
  data32 <<= 8;
  data32 |= _wire->read();
  data32 <<= 8;
  data32 |= _wire->read();
  return (data32 << 8) >> 8;
}

void ADS1219::start(){
  _wire->beginTransmission(address);
  _wire->write(0x08);
  _wire->endTransmission();
}

void ADS1219::powerDown(){
  _wire->beginTransmission(address);
  _wire->write(0x02);
  _wire->endTransmission();
}

void ADS1219::calibrate(){
  uint64_t calibrate = 0;
  for (int i = 0; i <  10; i++){
    calibrate += readShorted();
  }
  calibration = uint32_t (calibrate / 10);
}

uint8_t ADS1219::readRegister(uint8_t reg){
  _wire->beginTransmission(address);
  _wire->write(reg);
  _wire->endTransmission();
  _wire->requestFrom((uint8_t)address,(uint8_t)1);
  return _wire->read();
}

void ADS1219::writeRegister(uint8_t data){
  _wire->beginTransmission(address);
  _wire->write(CONFIG_REGISTER_ADDRESS);
  _wire->write(data);
  _wire->endTransmission();
}

long ADS1219::readConversionResult(){
  _wire->beginTransmission(address);
  _wire->write(0x10);
  _wire->endTransmission();
  _wire->requestFrom((uint8_t)address, (uint8_t)3);
  long data32 = _wire->read();
  data32 <<= 8;
  data32 |= _wire->read();
  data32 <<= 8;
  data32 |= _wire->read();
  return ((data32 << 8) >> 8) - calibration;
}

void ADS1219::resetConfig(){
	writeRegister(0x00);
}

long ADS1219::readData(uint8_t channel){
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
	  while(digitalRead(_rdy_pin)==1){
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
  while(digitalRead(_rdy_pin)==1);
  return readConversionResult();
}

long ADS1219::readDifferential_2_3(){
  config &= MUX_MASK;
  config |= MUX_DIFF_2_3;
  writeRegister(config);
  start();
  while(digitalRead(_rdy_pin)==1);
  return readConversionResult();
}

long ADS1219::readDifferential_1_2(){
  config &= MUX_MASK;
  config |= MUX_DIFF_1_2;
  writeRegister(config);
  start();
  while(digitalRead(_rdy_pin)==1);
  return readConversionResult();
}

long ADS1219::readShorted(){
  config &= MUX_MASK;
  config |= MUX_SHORTED;
  writeRegister(config);
  while(digitalRead(_rdy_pin)==1);
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
