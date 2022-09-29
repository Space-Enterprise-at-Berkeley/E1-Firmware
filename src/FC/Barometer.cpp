#include "Barometer.h"

namespace Barometer {
    BMP388_DEV bmp388_dev;
    float altitude = 0;
    float pressure = 0;
    
    void init() {
        bmp388_dev.begin();
    }

    uint32_t sampleBarometer() {
        uint8_t altCheck = bmp388_dev.getAltitude(altitude);
        uint8_t presCheck = bmp388_dev.getPressure(pressure);

        if (altCheck == 1 && presCheck == 1) {
            Comms::Packet barometerPacket = { .id = 13, .len = 0};
		    Comms::packetAddFloat(&barometerPacket, altitude);
            Comms::packetAddFloat(&barometerPacket, pressure);
		    Comms::emitPacket(&barometerPacket);
        }

		// 80 hz
		return updatePeriod;
    }
}