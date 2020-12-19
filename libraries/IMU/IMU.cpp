/*
  IMU.cpp - A c++ library to interface with the Arduino BNO055 IMU.
  Created by Vamshi Balanaga, Sept 8, 2020.
*/

#include "IMU.h"

// ==================== NEED TO TEST =======================================

IMU::IMU(uint8_t i2c_addr){
  _bno = Adafruit_BNO055(-1, i2c_addr);
  init();
}

void IMU::init(){
  if(!_bno.begin()){
    exit(1);
  }
  _bno.setExtCrystalUse(true);
}

void IMU::readAccelerationData(float *data){
  imu::Vector<3> accelData = _bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

  data[0] = accelData.x();
  data[1] = accelData.y();
  data[2] = accelData.z();
  data[3] = -1;
}

void IMU::readOrientationData(float *data){
  imu::Vector<3> orientationData = _bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  data[0] = oreintationData.x();
  data[1] = orientationData.y();
  data[2] = oreintationData.z();
  data[3] = -1;
}

void IMU::readOrientationChangeData(float *data){
  imu::Vector<3> gyroscopeData = _bno.getVector(Adafruit_BNO055::VECTOR::GYROSCOPE);

  data[0] = gyroscopeData.x();
  data[1] = gyroscopeData.y();
  data[2] = gyroscopeData.z();
  data[3] = -1;
}
