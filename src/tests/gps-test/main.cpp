#include <Arduino.h>
#include "GPS.h"
#include <Wire.h>
#define spiPort SPI //TODO: Update this to right pin

SFE_UBLOX_GNSS myGNSS;
uint32_t gpsUpdatePeriod = 40 * 1000;
double latitude;
double longitude;
double altitude;
double speed;
double degree;
byte SIV;

const uint8_t csPin = 10;  //TODO: Update this to right pin


int main() {
    Serial.begin(9600);
    Wire.begin();
    Serial.println("GPS setup");

    // Connect to the u-blox module using SPI port, csPin and speed setting
    if (!myGNSS.begin(spiPort, csPin, 5000000)){
        Serial.println("GPS failed to start");
    } else {
        Serial.println("GPS started");
    }
    
    // myGNSS.setI2COutput(COM_TYPE_UBX);   //Set the I2C port to output UBX only (turn off NMEA noise)
    myGNSS.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX);
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

    while (1)   {
        Serial.println();

        //Get Longtitude
        latitude = myGNSS.getLatitude() / 10000000;
        Serial.print(F("Lat: "));
        Serial.print(latitude);

        //Get Latitude
        longitude = myGNSS.getLongitude() / 10000000;
        Serial.print(F(" Long: "));
        Serial.print(longitude);

        // Serial.print(F(" (degrees"));

        //Get Altitude
        altitude = myGNSS.getAltitude();
        Serial.print(F(" Alt: "));
        Serial.print(altitude);
        Serial.print(F(" (mm)"));

        //Get SIV
        SIV = myGNSS.getSIV();
        Serial.print(F(" SIV: "));
        Serial.print(SIV);

        //Get Speed
        speed = myGNSS.getGroundSpeed();
        Serial.print(F(" speed: "));    
        Serial.print(speed / 1000); 
        Serial.print(F(" m/s"));

        //Get Degree
        degree = myGNSS.getHeading() / 100000;
        Serial.print(F(" Angle: "));    
        Serial.print(degree); 



    }
}
