#include "Ereg.h"

namespace EReg {

    char packetBuffer[sizeof(Comms::Packet)];

    float pt1Value = 0;
    float pt2Value = 0;
    float pt3Value = 0;
    float motorPosition = 0;

    void initEReg() {
        // EReg board connected to Serial8 of AC Teensy
        Serial8.begin(9600);

        Comms::registerCallback(184, zero);
        Comms::registerCallback(185, setPressureSetpoint);
        Comms::registerCallback(186, regulation);
        Comms::registerCallback(187, flow);
        Comms::registerCallback(188, fullOpen);
    }

    Comms::Packet eRegZeroPacket = {.id = 0};
    void zero(Comms::Packet tmp, uint8_t ip) {
        eRegZeroPacket.data[0] = tmp.data[0];
        sendToEReg(eRegZeroPacket);
    }
    
    Comms::Packet eRegPressureSetpointPacket = {.id = 1};
    void setPressureSetpoint(Comms::Packet tmp, uint8_t ip) {
        for (int i = 0; i < 4; i++) {
            eRegPressureSetpointPacket.data[i] = tmp.data[i];
        }
        sendToEReg(eRegPressureSetpointPacket);
    }
    
    Comms::Packet eRegRegulationPacket = {.id = 2};
    void regulation(Comms::Packet tmp, uint8_t ip) {
        eRegRegulationPacket.data[0] = tmp.data[0];
        sendToEReg(eRegRegulationPacket);
    }

    Comms::Packet eRegFlowPacket = {.id = 3};
    void flow(Comms::Packet tmp, uint8_t ip) {
        eRegFlowPacket.data[0] = tmp.data[0];
        sendToEReg(eRegFlowPacket);
    }

    Comms::Packet eRegFullOpenPacket = {.id = 4};
    void fullOpen(Comms::Packet tmp, uint8_t ip) {
        sendToEReg(eRegFlowPacket);
    }

    uint32_t sampleEregReadings() {
        Comms::Packet tmp = {.id = 85};
        Comms::packetAddFloat(&tmp, pt1Value);
        Comms::packetAddFloat(&tmp, pt2Value);
        Comms::packetAddFloat(&tmp, pt3Value);
        Comms::packetAddFloat(&tmp, motorPosition);
        Comms::emitPacket(&tmp);

        return samplePeriod;
    }

    void sendToEReg(Comms::Packet *packet) {
        //add timestamp to struct
        uint32_t timestamp = millis();
        packet->timestamp[0] = timestamp & 0xFF;
        packet->timestamp[1] = (timestamp >> 8) & 0xFF;
        packet->timestamp[2] = (timestamp >> 16) & 0xFF;
        packet->timestamp[3] = (timestamp >> 24) & 0xFF;

        //calculate and append checksum to struct
        uint16_t checksum = computePacketChecksum(packet);
        packet->checksum[0] = checksum & 0xFF;
        packet->checksum[1] = checksum >> 8;

        //send to EReg board
        Serial8.write(packet->id);
        Serial8.write(packet->len);
        Serial8.write(packet->timestamp, 4);
        Serial8.write(packet->checksum, 2);
        Serial8.write(packet->data, packet->len);
        Serial8.write('\n');   
    }

    uint32_t receiveFromEreg() {
        if(Serial.available()) {
            int cnt = 0;
            while(Serial.available() && cnt < sizeof(Comms::Packet)) {
                packetBuffer[cnt] = Serial.read();
                cnt++;
            }
            Comms::Packet *packet = (Comms::Packet *)&packetBuffer;
            
            pt1Value = Comms::packetGetFloat(packet, 0);
            pt2Value = Comms::packetGetFloat(packet, 4);
            pt3Value = Comms::packetGetFloat(packet, 8);
            motorPosition = Comms::packetGetFloat(packet, 12);
        }

        return samplePeriod;
    }

}