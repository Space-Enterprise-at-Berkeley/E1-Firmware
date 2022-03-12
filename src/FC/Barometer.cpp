#include "Barometer.h"

namespace Barometer {
    uint32_t bmUpdatePeriod = 30 * 1000; // TODO: Placeholder
    Comms::Packet baroPacket = {.id = 5};
    
    float baroAltitude, baroPressure, baroTemperature;

    void init() {
        Comms::registerEmitter({.packet = &baroPacket, .updatePeriod = bmUpdatePeriod});
        BlackBox::registerEmitter({.packet = &baroPacket, .updatePeriod = bmUpdatePeriod});
    }
    
    uint32_t sampleAltPressTemp() {
        HAL::bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
        bool successfully_measured = false;
        while (!successfully_measured) {
            successfully_measured = HAL::bmp388.getMeasurements(baroTemperature, baroPressure, baroAltitude);
        }

        //update packet
        baroPacket.len = 0;
        Comms::packetAddFloat(&baroPacket, baroAltitude);
        Comms::packetAddFloat(&baroPacket, baroPressure);
        Comms::packetAddFloat(&baroPacket, baroTemperature);

        return bmUpdatePeriod;
    }
}