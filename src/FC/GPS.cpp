#include "GPS.h"
#include <Wire.h>
#define spiPort SPI  //TODO: Update this to right pin 


namespace GPS {

    SFE_UBLOX_GNSS myGNSS;
    uint32_t gpsUpdatePeriod = 40 * 1000;

    Comms::Packet latLongPacket = {.id = 6};
    double latitude;
    double longitude;


    Comms::Packet auxPacket = {.id = 7};
    double altitude;
    double speed;
    double angle;
    byte SIV;

    const uint8_t csPin = 10;  //TODO: Update this to right pin

    //
    void initGPS() {
        Wire.begin();
        DEBUG("GPS setup");
        // Connect to the u-blox module using SPI port, csPin and speed setting
        if (!myGNSS.begin(spiPort, csPin, 5000000)){
            DEBUG("GPS failed to start");
        } else {
           DEBUG("GPS started");
        }
        
        // myGNSS.setI2COutput(COM_TYPE_UBX);   //Set the I2C port to output UBX only (turn off NMEA noise)
        myGNSS.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX);
        myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

    }

    uint32_t latLongSample() {

        //Get Longtitude
        latitude = myGNSS.getLatitude() / 10000000;
        DEBUG(F("Lat: "));
        DEBUG(latitude);

        //Get Latitude
        longitude = myGNSS.getLongitude() / 10000000;
        DEBUG(F(" Long: "));
        DEBUG(longitude);
        DEBUG(F(" (degrees"));

        Comms::packetAddFloat(&latLongPacket, latitude);
        Comms::packetAddFloat(&latLongPacket, longitude);
        Comms::emitPacket(&latLongPacket);

        return gpsUpdatePeriod;
    }

    
    uint32_t auxSample() {
        //Get Altitude
        altitude = myGNSS.getAltitude();
        DEBUG(F(" Alt: "));
        DEBUG(altitude);
        DEBUG(F(" (mm)"));

        //Get SIV
        SIV = myGNSS.getSIV();
        DEBUG(F(" SIV: "));
        DEBUG(SIV);

        //Get Angle
        angle = myGNSS.getHeading() / 100000;
        DEBUG(F(" Angle: "));    
        DEBUG(degree); 


        //Get Speed
        speed = myGNSS.getGroundSpeed();
        DEBUG(F(" Speed: "));
        DEBUG(speed);



        Comms::packetAddFloat(&auxPacket, altitude);
        Comms::packetAddFloat(&auxPacket, SIV);
        Comms::emitPacket(&auxPacket);

        return gpsUpdatePeriod;
    }
}



