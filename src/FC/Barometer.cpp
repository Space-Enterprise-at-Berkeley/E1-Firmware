#include "Barometer.h"

namespace Barometer {
    float altitude, pressure, temperature;
    BMP388_DEV bmp388;

    // Comms::Packet baroPacket = {.id = 21}; // TODO: check if ID is valid
    // uint32_t baroUpdatePeriod = 100 * 1000; // TODO: change placeholder

    void init() {
        bmp388.begin();
    }
    
    uint32_t sampleAltPressTemp() {
        bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
        bool successfully_measured = bmp388.getMeasurements(temperature, pressure, altitude);
        if (successfully_measured) {
            // baroPacket.len = 0;
            // Comms::packetAddFloat(&baroPacket, altitude);
            // Comms::packetAddFloat(&baroPacket, pressure);
            // Comms::packetAddFloat(&baroPacket, temperature);
            // Comms::emitPacket(&baroPacket);
            Serial.print("temperature: ");
            Serial.print(temperature);
            Serial.print(", pressure: ");
            Serial.print(pressure);
            Serial.print(", altitude: ");
            Serial.println(altitude);

            // return baroUpdatePeriod;
            return 0;
        } else {
            // TODO
            Serial.println("FAIL!!");
        }
    }
}