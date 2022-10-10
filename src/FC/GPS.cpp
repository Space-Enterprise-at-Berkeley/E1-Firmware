#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include "GPS.h"
namespace GPS { 

  uint32_t gpsUpdatePeriod = 100 * 1000;

  Comms::Packet latLongPacket = {.id = 6};

  float latitude;
  float longitude;
  float altitude;
  float speed;
  float angle;
  uint8_t satFixNum;
  uint8_t fixType;

  void initGPS() { 
      HAL::neom9n.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX); //gSet the SPI port to output UBX only (turn off NMEA noise)
      HAL::neom9n.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
      HAL::neom9n.setNavigationFrequency(10);
      HAL::neom9n.setAutoPVT(true);
  }

  uint32_t latLongSample() {
    
    latitude = (float)(HAL::neom9n.getLatitude()) / 10e6;

    longitude = (float)(HAL::neom9n.getLongitude()) / 10e6;

    altitude = (float)(HAL::neom9n.getAltitude()) / 10e3; // raw is mm

    speed = (float)(HAL::neom9n.getSpeedAccEst()) / 10e3; // raw is mm/s

    satFixNum = HAL::neom9n.getSIV();

    // 0=no, 3=3D, 4=GNSS+Deadreckoning
    fixType = HAL::neom9n.getFixType();

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

