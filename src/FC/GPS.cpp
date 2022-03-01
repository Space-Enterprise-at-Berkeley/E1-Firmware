#include "GPS.h"
#include <Wire.h>


namespace GPS {

    SFE_UBLOX_GNSS myGNSS;
    uint32_t gpsUpdatePeriod = 40 * 1000;

    Comms::Packet latLongPacket = {.id = 11};
    double latitude;
    double longitude;


    Comms::Packet auxPacket = {.id = 12};
    double altitude;
    double speed;
    double angle;
    byte SIV;

    //
    void initGPS() {
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
    }

    uint32_t latLongSample() {

        Serial.println();
        //Get Longtitude
        latitude = myGNSS.getLatitude() / 10000000;
        Serial.print(F("Lat: "));
        Serial.print(latitude);

        //Get Latitude
        longitude = myGNSS.getLongitude() / 10000000;
        Serial.print(F(" Long: "));
        Serial.print(longitude);
        Serial.print(F(" (degrees"));

        Comms::packetAddFloat(&latLongPacket, latitude);
        Comms::packetAddFloat(&latLongPacket, longitude);
        Comms::emitPacket(&latLongPacket);

        return gpsUpdatePeriod;
    }

    
    uint32_t auxSample() {
        //Get Altitude
        altitude = myGNSS.getAltitude();
        Serial.print(F(" Alt: "));
        Serial.print(altitude);
        Serial.print(F(" (mm)"));

        //Get SIV
        SIV = myGNSS.getSIV();
        Serial.print(F(" SIV: "));
        Serial.print(SIV);

        //Get Angle


        //Get Speed

        Comms::packetAddFloat(&auxPacket, altitude);
        Comms::packetAddFloat(&auxPacket, SIV);
        Comms::emitPacket(&auxPacket);

        return gpsUpdatePeriod;
    }
}



