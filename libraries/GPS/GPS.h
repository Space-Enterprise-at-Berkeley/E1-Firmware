/*
  GPS.h - A c++ library to interface with the Arduino Ultimate GPS MTK3339.
  Created by Vamshi Balanaga, Aug 21, 2020.
*/

#include <Adafruit_GPS.h>
// #include "TimerInterrupt.h"

using namespace std;


namespace GPS {
  #define GPSECHO false

  Adafruit_GPS _gps;
  uint8_t commMethod; // 1 for HardwareSerial, 2 for SoftwareSerial, 3 for I2c, 4 for SPI


  void init() {
      _gps.begin(9600);

      _gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
      _gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
      _gps.sendCommand(PGCMD_ANTENNA);

      delay(1000);
  }

  void init(HardwareSerial *ser) {
     _gps = Adafruit_GPS(ser);
    commMethod = 1;
    init();
  }

  void init(TwoWire *theWire) {
     _gps = Adafruit_GPS(theWire);
    commMethod = 3;
    init();
  }

  void init(SPIClass *theSPI, int8_t cspin) {
    _gps = Adafruit_GPS(theSPI, cspin);
    commMethod = 4;
    init();
  }

  bool dataAvailable() {
    return _gps.newNMEAreceived();
  }

  bool gotSatelliteFix() {
     _gps.read();
     return _gps.fix;
  }

  void readPositionData(float *data) {
    _gps.read();
    data[0] = _gps.latitudeDegrees;
    data[1] = _gps.longitudeDegrees;
    data[2] = -1;
  }

  /**
   * Define this in advance. Need to be agreed on by everyone.
   */
  void readAuxilliaryData(float *data) {
    _gps.read();
    data[0] = _gps.altitude;
    data[1] = _gps.speed;
    data[2] = _gps.angle;
    data[3] = _gps.satellites;
    data[4] = -1;
  }
};
