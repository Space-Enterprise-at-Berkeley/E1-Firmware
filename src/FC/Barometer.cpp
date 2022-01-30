#include "Barometer.h"

namespace Barometer {
    uint32_t bmUpdatePeriod = 100 * 1000; // TODO: Placeholder
    Comms::Packet baroPacket = {.id = 5};
    
    float baroAltitude, baroPressure, baroTemperature;

    void init() {
    }
    
    uint32_t sampleAltPressTemp() {
        HAL::bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
        bool successfully_measured = false;
        while (!successfully_measured) {
            successfully_measured = HAL::bmp388.getMeasurements(baroTemperature, baroPressure, baroAltitude);
        }
        baroPacket.len = 0;
        Comms::packetAddFloat(&baroPacket, baroAltitude);
        Comms::packetAddFloat(&baroPacket, baroPressure);
        Comms::packetAddFloat(&baroPacket, baroTemperature);
        Comms::emitPacket(&baroPacket);
        return bmUpdatePeriod;
    }
}