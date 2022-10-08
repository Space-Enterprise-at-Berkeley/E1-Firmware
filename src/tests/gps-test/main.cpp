#include <Arduino.h>
#include <NEOM9N.h>
#include <SPI.h>


SFE_UBLOX_GNSS myGNSS;
uint32_t gpsUpdatePeriod = 40 * 1000;
uint8_t gpsCS = 38;

double latitude;
double longitude;
double altitude;
double speed;
double degree;
byte SIV;

int main() {
    Serial.begin(9600);

    Serial.println("GPS setup");
    if (!myGNSS.begin(&SPI, gpsCS, 4000000)) {
        Serial.println("GPS failed to start");
    } else {
        Serial.println("GPS started");
    }

  myGNSS.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX); //Set the SPI port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

    while (1)   {
        Serial.println();

        latitude = myGNSS.getLatitude() / 10000000;
        Serial.print(F("Lat: "));
        Serial.print(latitude);

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