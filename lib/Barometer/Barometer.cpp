/*
  IMU.cpp - A c++ library to interface with the Arduino BMP3XX Barometer.
  Created by Nolan Lautrette, March 21, 2021.
*/

#ifndef __BMP__
#define __BMP__

#include "Barometer.h"


float seaLevelPressure;
bool updatedSeaLevelPressure;

Barometer::Barometer() {
}

void Barometer::init(TwoWire *theWire) {

  _bmp = Adafruit_BMP3XX();

  seaLevelPressure = SEALEVELPRESSURE_HPA;
  updatedSeaLevelPressure = false;

  if(!_bmp.begin_I2C(BMP3XX_DEFAULT_ADDRESS, theWire)){
    exit(1);
  }
  _bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_4X);
  _bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
  _bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  _bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void Barometer::readAltitudeData(float *data) {
  data[0] = _bmp.readAltitude(seaLevelPressure);
  data[1] = -1;
}

void Barometer::readPressureData(float *data) {
  data[0] = _bmp.readPressure();
  data[1] = -1;
}

void Barometer::readTemperatureData(float *data) {
  data[0] = _bmp.readTemperature();
  data[1] = -1;
}

void Barometer::readAllData(float *data) {
  data[0] = _bmp.readAltitude(seaLevelPressure);
  data[1] = _bmp.readPressure();
  data[2] = -1;
}

bool Barometer::updateSeaLevelPressure(float newPressure) {
  seaLevelPressure = newPressure;
  updatedSeaLevelPressure = true;
  return updatedSeaLevelPressure;
}

bool Barometer::getSeaLevelStatus(){
  return updatedSeaLevelPressure;
}

#endif
