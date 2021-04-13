/*
  GPS.h - A c++ library to interface with the Arduino Ultimate GPS MTK3339.
  Created by Vamshi Balanaga, Aug 21, 2020.
*/

#include <Adafruit_GPS.h>
// #include "TimerInterrupt.h"

using namespace std;

class GPS {
  public:
    GPS(uint8_t i2c_addr);
    GPS(HardwareSerial &ser);
    //GPS(SoftwareSerial &ser);
    GPS(TwoWire *theWire);
    GPS(SPIClass *theSPI, int8_t cspin);
    bool dataAvailable();
    bool gotSatelliteFix();
    void readPositionData(float *data);
    void readAuxilliaryData(float *data);
    void init();
    char readChar();
    void checkNMEA();

  private:
    Adafruit_GPS _gps;
    //void init();
};
