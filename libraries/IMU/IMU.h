/*
  IMU.h - A c++ library to interface with the Arduino BNO055 IMU.
  Created by Vamshi Balanaga, Sept 8, 2020.
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

using namespace std;

#define BNO055_SAMPLERATE_DELAY_MS (100)


class IMU {
  public:
    IMU();
    void readAccelerationData(float *data);
    void readOrientationData(float *data);
    void readOrientationChangeData(float *data);

  private:
    Adafruit_BNO055 _bno;
    void init();
}
