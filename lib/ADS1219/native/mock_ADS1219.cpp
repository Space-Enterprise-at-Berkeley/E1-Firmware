#include <Wire.h>
#include "ADS1219.h"

ADS1219::ADS1219(int drdy, uint8_t addr, TwoWire *wire) {
  // do nothing.
}

ADS1219::ADS1219() {
  // do nothing.
}

void ADS1219::init(int drdy, uint8_t addr, TwoWire *wire) {
  // do nothing.
}

long ADS1219::getData(uint8_t conf) {
  // do nothing.
  return 0;
}

void ADS1219::start(){
  // do nothing.
}

void ADS1219::powerDown(){
  // do nothing.
}

void ADS1219::calibrate(){
  // do nothing
}

uint8_t ADS1219::readRegister(uint8_t reg){
  return 0;
}

void ADS1219::writeRegister(uint8_t data){
  //do nothing
}

long ADS1219::readConversionResult(){
  return 0;
}

void ADS1219::resetConfig(){
  //do nothing
}

long ADS1219::readData(int channel){
	  return readConversionResult();
}

long ADS1219::readDifferential_0_1(){
  return readConversionResult();
}

long ADS1219::readDifferential_2_3(){
  return readConversionResult();
}

long ADS1219::readDifferential_1_2(){
  return readConversionResult();
}

long ADS1219::readShorted(){
  return readConversionResult();
}

void ADS1219::setGain(adsGain_t gain){
  //do nothing
}

void ADS1219::setDataRate(int rate){
	//do nothing.
}

void ADS1219::setConversionMode(adsMode_t mode){
  //do nothing
}

void ADS1219::setVoltageReference(adsRef_t vref){
  //do nothing
}
