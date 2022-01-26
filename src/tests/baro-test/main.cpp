#include <Common.h>
// #include <Comms.h>
#include "BMP388_DEV.h"

int main() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("starting");
    Serial.flush();
    float altitude, pressure, temperature;
    BMP388_DEV bmp388;
    bmp388.begin();
    bool successfully_measured;

    while(1) {
        bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
        successfully_measured = false;
        while (!successfully_measured) {
            successfully_measured = bmp388.getMeasurements(temperature, pressure, altitude);
        }
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
        delay(1000);
    }
    
    return 0;
}
