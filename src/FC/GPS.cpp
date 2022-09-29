#include "GPS.h"

namespace GPS {
   
    SFE_UBLOX_GNSS neom9n;

    void initGPS() {
        neom9n.begin();
    };

    uint32_t sampleGPS() {
        neom9n.checkUblox();
        float latitude = neom9n.getLatitude();
        float longitude = neom9n.getLongitude(); 
        float altitude = neom9n.getAltitude();
        float speed = neom9n.getSpeedAccEst();


        Comms::Packet gpsData = {.id = 6, .len = 0};
        Comms::packetAddFloat(&gpsData, latitude);
        Comms::packetAddFloat(&gpsData, longitude);
        Comms::packetAddFloat(&gpsData, altitude);
        Comms::packetAddFloat(&gpsData, speed);

        Comms::emitPacket(&gpsData);


        return 100 * 1000;
    }
}