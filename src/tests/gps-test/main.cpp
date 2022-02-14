#include "GPS.h"
#include <Wire.h>

SFE_UBLOX_GNSS myGNSS;
uint32_t gpsUpdatePeriod = 40 * 1000;
long latitude = 0;
long longitude = 0;
long altitude = 0;
byte SIV = 0;

int main() {
    Serial.begin(9600);
    Wire.begin();
    Serial.println("GPS setup");
    if (!myGNSS.begin()) {
        Serial.println("GPS failed to start");
    } else {
        Serial.println("GPS started");
    }
    // Version 2 - UBX
    myGNSS.setI2COutput(COM_TYPE_UBX);   //Set the I2C port to output UBX only (turn off NMEA noise)
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

    while (1)   {
        Serial.println();
        //Get Longtitude
        latitude = myGNSS.getLatitude();
        Serial.print(F("Lat: "));
        Serial.print(latitude);

        //Get Latitude
        longitude = myGNSS.getLongitude();
        Serial.print(F(" Long: "));
        Serial.print(longitude / 10000000);
        Serial.print(F(" (degrees"));

        //Get Altitude
        altitude = myGNSS.getAltitude();
        Serial.print(F(" Alt: "));
        Serial.print(altitude);
        Serial.print(F(" (mm)"));

        //Get SIV
        SIV = myGNSS.getSIV();
        Serial.print(F(" SIV: "));
        Serial.print(SIV);
    }
}
