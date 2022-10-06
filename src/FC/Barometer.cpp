#include "Barometer.h"

namespace Barometer {
    BMP388_DEV bmp388_dev;
    float altitude = 0;
    float pressure = 0;
    float temperature = 0;
    uint32_t updatePeriod = 10 * 1000;
    
    void init() {
        bmp388_dev.begin();
    }

    uint32_t getUpdatePeriod() {
        return updatePeriod;
    }

    uint32_t sampleBarometer() {
        uint8_t altCheck = bmp388_dev.getAltitude(altitude);
        uint8_t presCheck = bmp388_dev.getPressure(pressure);
        uint8_t tempCheck = bmp388_dev.getTemperature(temperature);

        if (altCheck == 1 && presCheck == 1) {
            Comms::Packet barometerPacket = { .id = 5, .len = 0};
		    Comms::packetAddFloat(&barometerPacket, altitude);
            Comms::packetAddFloat(&barometerPacket, pressure);
            Comms::packetAddFloat(&barometerPacket, temperature);
		    Comms::emitPacket(&barometerPacket);
        }

		// 100 hz
		return updatePeriod;
    }
    
    float getAltitude() {
        return altitude;
    }

}