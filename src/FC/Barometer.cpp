#include "Barometer.h"

namespace Barometer {
    uint32_t bmUpdatePeriod = 100 * 1000; // TODO: Placeholder
    Comms::Packet baroPacket = {.id = 5};
    
    float baroAltitude, baroPressure;

    void init() {
    }
    
    uint32_t sampleAltPress() {
        float _ = 0;
        HAL::bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
        bool successfully_measured = false;
        while (!successfully_measured) {
            successfully_measured = HAL::bmp388.getMeasurements(_, baroPressure, baroAltitude);
        }
        baroPacket.len = 0;
        Comms::packetAddFloat(&baroPacket, baroAltitude);
        Comms::packetAddFloat(&baroPacket, baroPressure);
        Comms::emitPacket(&baroPacket);
        // Serial.print("altitude: ");
        // Serial.print(altitude);
        // Serial.print(", pressure: ");
        // Serial.print(pressure);
        // Serial.println();
        return bmUpdatePeriod;
    }
}