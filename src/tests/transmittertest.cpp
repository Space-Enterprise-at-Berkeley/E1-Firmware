#include <Arduino.h>

#include <Comms.h>

#define RS485_SW_PIN 22 

void setup() {
    Serial.begin(921600);
    pinMode(RS485_SW_PIN, OUTPUT);
    digitalWrite(RS485_SW_PIN, HIGH);
    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);
}

void loop() {
    // Comms::Packet p = {.id = 38};
    // Comms::packetAddFloat(&p, 2.3);
    // Comms::packetAddFloat(&p, 4.5);
    // Comms::packetAddFloat(&p, 6.7);

    // uint32_t timestamp = millis();
    // p.timestamp[0] = timestamp & 0xFF;
    // p.timestamp[1] = (timestamp >> 8) & 0xFF;
    // p.timestamp[2] = (timestamp >> 16) & 0xFF;
    // p.timestamp[3] = (timestamp >> 24) & 0xFF;

    // //calculate and append checksum to struct
    // uint16_t checksum = Comms::computePacketChecksum(&p);
    // p.checksum[0] = checksum & 0xFF;
    // p.checksum[1] = checksum >> 8;

    // Comms::emitPacket(&p, &Serial);
    for(int i = 0; i < 3000; i++) {
        Serial.write('\n');
    }
    Serial.flush();

    delay(30);
}
