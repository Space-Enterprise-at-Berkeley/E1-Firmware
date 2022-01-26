#include "Barometer.h"

namespace Barometer {
    float altitude, pressure, temperature;
    BMP388_DEV bmp388;

    uint32_t bmUpdatePeriod = 100 * 1000; // TODO: Placeholder
    Comms::Packet baroPacket = {.id = 13};

    void init() {
        bmp388.begin();
    }
    
    uint32_t sampleAltPressTemp() {
        bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
        bool successfully_measured = false;
        while (!successfully_measured) {
            successfully_measured = bmp388.getMeasurements(temperature, pressure, altitude);
        }
        baroPacket.len = 0;
        Comms::packetAddFloat(&baroPacket, altitude);
        Comms::packetAddFloat(&baroPacket, pressure);
        Comms::packetAddFloat(&baroPacket, temperature);
        Comms::emitPacket(&baroPacket);
        // Serial.print("temperature: ");
        // Serial.print(temperature);
        // Serial.print(", pressure: ");
        // Serial.print(pressure);
        // Serial.print(", altitude: ");
        // Serial.println(altitude);
        return bmUpdatePeriod;
    }
}