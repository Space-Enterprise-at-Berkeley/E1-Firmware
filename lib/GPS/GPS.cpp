/*
  GPS.cpp - A c++ library to interface with the Arduino  GPS.
  Created by Nolan Lautrette, Feb 15, 2021.
*/

#ifndef __GPS__
#define __GPS__


#include "GPS.h"

int commMethod;
Adafruit_GPS _gps;

GPS::GPS(HardwareSerial &ser) {
  _gps = Adafruit_GPS(&ser);
  commMethod = 1;
  //init();
}
/*
GPS::GPS(SoftwareSerial &ser) {
  _gps = Adafruit_GPS(&ser);
  commMethod = 2;
  init();
}
*/
GPS::GPS(TwoWire *theWire) {
  _gps = Adafruit_GPS(theWire);
  commMethod = 3;
  init();
}

GPS::GPS(SPIClass *theSPI, int8_t cspin) {
  _gps = Adafruit_GPS(theSPI, cspin);
  commMethod = 4;
  init();
}

void GPS::init() {
    _gps.begin(9600);

    _gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    _gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
    _gps.sendCommand(PGCMD_ANTENNA);

    delay(1000);
  }

bool GPS::dataAvailable() {
    return _gps.newNMEAreceived();
  }

bool GPS::gotSatelliteFix() {
   _gps.read();
   return _gps.fix;
}

void GPS::readPositionData(float *data) {
  _gps.read();
  data[0] = _gps.latitudeDegrees;
  data[1] = _gps.longitudeDegrees;
  data[2] = -1;
}

/**
 * Define this in advance. Need to be agreed on by everyone.
 */
void GPS::readAuxilliaryData(float *data) {
  _gps.read();
  data[0] = _gps.altitude;
  data[1] = _gps.speed;
  data[2] = _gps.angle;
  data[3] = _gps.satellites;
  data[4] = -1;
}





#endif
