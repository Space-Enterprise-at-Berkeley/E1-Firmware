#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include "GPS.h"

namespace GPS { 

  uint32_t gpsUpdatePeriod = 100 * 1000;

  SFE_UBLOX_GNSS neom9n; 
  Comms::Packet latLongPacket = {.id = 6};

  float latitude;
  float longitude;
  float altitude;
  float speed;
  float angle;
  uint8_t satFixNum;
  uint8_t fixType;

  void init() { 
    SPI.begin();
    if(!neom9n.begin(SPI, 38, 4000000)) {
        DEBUG("GPS DIDN'T INIT");
        DEBUG("\n");
    } else {
        DEBUG("GPS INIT SUCCESSFUL");
        DEBUG("\n");
    }
      neom9n.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX); //gSet the SPI port to output UBX only (turn off NMEA noise)
      neom9n.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
      neom9n.setNavigationFrequency(10);
      neom9n.setAutoPVT(true);
  }

  uint32_t latLongSample() {
    
    latitude = (float)(neom9n.getLatitude()) / 10e6;

    longitude = (float)(neom9n.getLongitude()) / 10e6;

    altitude = (float)(neom9n.getAltitude()) / 10e3; // raw is mm

    speed = (float)(neom9n.getSpeedAccEst()) / 10e3; // raw is mm/s

    satFixNum = neom9n.getSIV();

    // 0=no, 3=3D, 4=GNSS+Deadreckoning
    fixType = neom9n.getFixType();

    DEBUG("GPS Latitude: ");
    DEBUG(latitude);
    DEBUG("     Longitude: ");
    DEBUG(longitude);
    DEBUG("     Altitude: ");
    DEBUG(altitude);
    DEBUG("     Speed: ");
    DEBUG(speed);
    DEBUG("     NumSatellites: ");
    DEBUG(satFixNum);
    DEBUG("     Type of fix:  ");
    DEBUG(fixType);
    DEBUG("\n");
    DEBUG_FLUSH();

    latLongPacket.len = 0;
    Comms::packetAddFloat(&latLongPacket, latitude);
    Comms::packetAddFloat(&latLongPacket, longitude);
    Comms::packetAddFloat(&latLongPacket, altitude);
    Comms::packetAddFloat(&latLongPacket, speed);
    Comms::packetAddUint8(&latLongPacket, fixType);
    Comms::packetAddUint8(&latLongPacket, satFixNum);

    Comms::emitPacket(&latLongPacket);

    return gpsUpdatePeriod;
  }

  uint32_t auxSample() {
    return gpsUpdatePeriod;
  }
}
