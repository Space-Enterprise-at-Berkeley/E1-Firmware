/*
  Barometer.h - A c++ library to interface with the Arduino BMP3XX Barometer.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>

using namespace std;


namespace Barometer {

  #define BNO055_SAMPLERATE_DELAY_MS (100)
  #define SEALEVELPRESSURE_HPA (1013.25)

  Adafruit_BMP3XX _bmp;
  float seaLevelPressure;
  bool updatedSeaLevelPressure;

  void init(TwoWire *theWire) {

    seaLevelPressure = SEALEVELPRESSURE_HPA;
    updatedSeaLevelPressure = false;

    _bmp = Adafruit_BMP3XX(-1);
    if(!_bmp.begin(BMP3XX_DEFAULT_ADDRESS, theWire)){
      exit(1);
    }
    _bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_4X);
    _bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
    _bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    _bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  }

  void readAltitudeData(float *data) {
    data[0] = _bmp.readAltitude(seaLevelPressure);
    data[1] = -1;
  }

  void readPressureData(float *data) {
    data[0] = _bmp.readPressure();
    data[1] = -1;
  }

  void readTemperatureData(float *data) {
    data[0] = _bmp.readTemperature();
    data[1] = -1;
  }

  bool updateSeaLevelPressure(float newPressure) {
    seaLevelPressure = newPressure;
    updatedSeaLevelPressure = true;
    return updateSeaLevelPressure;
  }

  bool getSeaLevelStatus(){
    return updatedSeaLevelPressure;
  }
};
