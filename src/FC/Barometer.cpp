#include "Barometer.h"

namespace Barometer {
    uint32_t bmUpdatePeriod = 30 * 1000; // TODO: Placeholder
    Comms::Packet baroPacket = {.id = 5};

    Task* _zeroAltitudeTask;

    float altitudeOffset = 0;
    
    float baroAltitude, baroPressure, baroTemperature;

    void init(Task *zeroAltitudeTask) {
        _zeroAltitudeTask = zeroAltitudeTask;
    }
    
    uint32_t sampleAltPressTemp() {
        HAL::bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
        bool successfully_measured = false;
        while (!successfully_measured) {
            successfully_measured = HAL::bmp388.getMeasurements(baroTemperature, baroPressure, baroAltitude);
        }
        baroPacket.len = 0;
        Comms::packetAddFloat(&baroPacket, baroAltitude - altitudeOffset);
        Comms::packetAddFloat(&baroPacket, baroPressure);
        Comms::packetAddFloat(&baroPacket, baroTemperature);
        Comms::emitPacket(&baroPacket);
        Comms::emitPacket(&baroPacket, &RADIO_SERIAL, "\r\n\n", 3);
        DEBUG("Barometer Altitude: ");
        DEBUG(baroAltitude - altitudeOffset);
        DEBUG("     Barometer Pressure: ");
        DEBUG(baroPressure);
        DEBUG("     Barometer Temperature: ");
        DEBUG(baroTemperature);
        DEBUG("\n");
        DEBUG_FLUSH();

        return bmUpdatePeriod;
    }

    float totalAltitude = 0;
    float samples = 0;

    uint32_t zeroAltitude() {

        // get readings
        float altitude;
        float pressure;
        float temperature;

        HAL::bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
        bool successfully_measured = false;
        while (!successfully_measured) {
            successfully_measured = HAL::bmp388.getMeasurements(temperature, pressure, altitude);
        }

        samples++;
        altitudeOffset = altitude;

        return 1000;
    }

    uint32_t getUpdatePeriod() { 
        return bmUpdatePeriod;
    }

    float getAltitude() { 
        return baroAltitude - altitudeOffset;
    }
}