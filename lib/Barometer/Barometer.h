/*
  Barometer.h - A c++ library to interface with the Arduino BMP3XX Barometer.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>

using namespace std;

#define BNO055_SAMPLERATE_DELAY_MS (100)
#define SEALEVELPRESSURE_HPA (1013.25)

class Barometer {
  public:
    void readAltitudeData(float *data);
    void readPressureData(float *data);
    void readTemperatureData(float *data);
    void readAllData(float *data);
    bool updateSeaLevelPressure(float newPressure);
    bool getSeaLevelStatus();
    void init(TwoWire *theWire);

  private:
    Adafruit_BMP3XX _bmp;
    //void init();
};
