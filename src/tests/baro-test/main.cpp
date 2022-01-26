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

    while(1) {
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
        } else {
            Serial.println("FAIL!!");
        }
        delay(1000);
    }
    
    return 0;
}
