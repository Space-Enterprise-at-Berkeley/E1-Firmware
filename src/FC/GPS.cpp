#include "GPS.h"
#include <Wire.h>


namespace GPS {

    SFE_UBLOX_GNSS myGNSS;
    uint32_t gpsUpdatePeriod = 40 * 1000;

    // Comms::Packet latLongPacket = {.id = 11};
    long latitude = 0;
    long longitude = 0;


    // Comms::Packet auxPacket = {.id = 12};
    long altitude = 0;
    byte SIV = 0;

    //
    void setup() {

        Serial.begin(9600);
        Wire.begin();
        Serial.println("GPS setup");
        if (!myGNSS.begin()) {
            Serial.println("GPS failed to start");
        } else {
            Serial.println("GPS started");
        }



        // Version 1 - NMEA 
        // // myGNSS.setxI2COutput(COM_TYPE_NMEA | COM_TYPE_UBX); // Turn on both UBX and NMEA sentences on I2C. (Turn off RTCM and SPARTN)
        // myGNSS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_I2C); // Several of these are on by default on ublox board so let's disable them
        // myGNSS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_I2C);
        // myGNSS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_I2C);
        // myGNSS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_I2C);
        // myGNSS.disableNMEAMessage(UBX_NMEA_VTG, COM_PORT_I2C);
        // myGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_I2C); // Leave only GGA enabled at current navigation rate

        // // Set the Main Talker ID to "GP". The NMEA GGA messages will be GPGGA instead of GNGGA
        // myGNSS.setMainTalkerID(SFE_UBLOX_MAIN_TALKER_ID_GP);

        // myGNSS.setHighPrecisionMode(true);

        // Version 2 - UBX
        myGNSS.setI2COutput(COM_TYPE_UBX);   //Set the I2C port to output UBX only (turn off NMEA noise)
        myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
        
    }

    void loop() {

        // Serial.println(123);
        // NMEA_GGA_data_t data; 
        // uint8_t result = myGNSS.getLatestNMEAGPGGA(&data);

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

        // Comms::packetAddFloat(&latLongPacket, latitude);
        // Comms::packetAddFloat(&latLongPacket, longitude);
        // Comms::emitPacket(&latLongPacket);

        //Get Altitude
        altitude = myGNSS.getAltitude();
        Serial.print(F(" Alt: "));
        Serial.print(altitude);
        Serial.print(F(" (mm)"));

        //Get SIV
        SIV = myGNSS.getSIV();
        Serial.print(F(" SIV: "));
        Serial.print(SIV);

        // Comms::packetAddFloat(&auxPacket, altitude);
        // Comms::packetAddFloat(&auxPacket, SIV);
        // Comms::emitPacket(&auxPacket);
        
        // return gpsUpdatePeriod;

    }
}



