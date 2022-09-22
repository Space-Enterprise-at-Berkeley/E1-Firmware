#include "GPS.h"


namespace GPS {

    uint32_t gpsUpdatePeriod = 100 * 1000;

    Comms::Packet latLongPacket = {.id = 6};
    float latitude;
    float longitude;
    float altitude;
    float speed;
    float angle;



    Comms::Packet auxPacket = {.id = 7};

    //
    void initGPS() {
        // Version 2 - UBX
        HAL::neom9n.setI2COutput(COM_TYPE_UBX);   //Set the I2C port to output UBX only (turn off NMEA noise)
        HAL::neom9n.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
        HAL::neom9n.setNavigationFrequency(10);
        HAL::neom9n.setAutoPVT(true);
    }

    uint32_t latLongSample() {
        //Get Longtitude
        latitude = (float)HAL::neom9n.getLatitude() / 10000000.0;

        //Get Latitude
        longitude = (float)HAL::neom9n.getLongitude() / 10000000.0;

        altitude = (float)HAL::neom9n.getAltitude() / 1000.0; // raw is mm

        speed = (float)HAL::neom9n.getSpeedAccEst() / 1000.0; // raw is mm/s


        latLongPacket.len = 0;
        Comms::packetAddFloat(&latLongPacket, latitude);
        Comms::packetAddFloat(&latLongPacket, longitude);
        Comms::packetAddFloat(&latLongPacket, altitude);
        Comms::packetAddFloat(&latLongPacket, speed);
        Comms::emitPacket(&latLongPacket);

        return gpsUpdatePeriod;
    }

    
    uint32_t auxSample() {
        //Get Altitude
        // altitude = HAL::neom9n.getAltitude();

        // //Get SIV
        // SIV = HAL::neom9n.getSIV();

        // //Get Angle


        // //Get Speed

        // auxPacket.len = 0;
        // Comms::packetAddFloat(&auxPacket, altitude);
        // Comms::packetAddFloat(&auxPacket, SIV);
        // Comms::emitPacket(&auxPacket);

        return gpsUpdatePeriod;
    }
}



