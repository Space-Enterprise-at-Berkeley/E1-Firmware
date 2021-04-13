/*
  IMU.cpp - A c++ library to interface with the Arduino BNO055 IMU.
  Created by Vamshi Balanaga, Sept 8, 2020.
*/
#ifndef __IMU1__
#define __IMU1__

//#include "IMU.h"

// ==================== NEED TO TEST =======================================
//namespace IMU1 {
#include "IMU.h"

IMU::IMU() {
}

void IMU::init(TwoWire *theWire){
  _bno = Adafruit_BNO055(55, BNO055_ADDRESS_A, theWire);
  if(!_bno.begin()){
    exit(1);
  }
  _bno.setExtCrystalUse(true);
  //_bno.setAxisRemap(adafruit_bno055_axis_remap_config_t(0b00000110));
  _bno.setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_P2);
}

void IMU::readAccelerationData(float *data){
  imu::Vector<3> accelData = _bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

  data[0] = accelData.x();
  data[1] = accelData.y();
  data[2] = accelData.z();
  data[3] = -1;
}

void IMU::readQuaternionData(float *data) {
  imu::Quaternion quat = _bno.getQuat();

  data[0] = quat.w() * 100000;
  data[1] = quat.x() * 100000;
  data[2] = quat.y() * 100000;
  data[3] = quat.z() * 100000;
  data[4] = -1;
}

void IMU::readOrientationData(float *data){
  imu::Vector<3> orientationData = _bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  data[0] = orientationData.x();
  data[1] = orientationData.y();
  data[2] = orientationData.z();
  data[3] = -1;
}

void IMU::readOrientationChangeData(float *data){
  imu::Vector<3> gyroscopeData = _bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

  data[0] = gyroscopeData.x();
  data[1] = gyroscopeData.y();
  data[2] = gyroscopeData.z();
  data[3] = -1;
}
//}
#endif
