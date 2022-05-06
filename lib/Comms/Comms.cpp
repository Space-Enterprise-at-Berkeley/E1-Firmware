#include <Comms.h>

namespace Comms {

    std::map<uint8_t, commFunction> callbackMap;
    char packetBuffer[sizeof(Packet)];

    void initComms() {

        registerCallback(0, sendFirmwareVersionPacket);
    }


    void registerCallback(uint8_t id, commFunction function) {
        callbackMap.insert(std::pair<int, commFunction>(id, function));
    }

    /**
     * @brief Checks checksum of packet and tries to call the associated callback function.
     * 
     * @param packet Packet to be processed.
     */
    void evokeCallbackFunction(Packet *packet) {
        uint16_t checksum = *(uint16_t *)&packet->checksum;
        if (checksum == computePacketChecksum(packet)) {
            // DEBUG("Packet with ID ");
            // DEBUG(packet->id);
            // DEBUG(" has correct checksum!\n");
            //try to access function, checking for out of range exception
            if(callbackMap.count(packet->id)) {
                callbackMap.at(packet->id)(*packet);
            } else {
                // DEBUG("ID ");
                // DEBUG(packet->id);
                // DEBUG(" does not have a registered callback function.\n");
            }
        } else {
            DEBUG("Packet with ID ");
            DEBUG(packet->id);
            DEBUG(" does not have correct checksum!\n");
        }
    }

    void processWaitingPackets() {
        // if(Udp.parsePacket()) {
        //     if(Udp.remotePort() != port) return; // make sure this packet is for the right port
        //     Udp.read(packetBuffer, sizeof(Packet));

        //     Packet *packet = (Packet *)&packetBuffer;
        //     DEBUG(packet->id);
        //     DEBUG("\n");
        //     // DEBUG("Got unverified packet with ID ");
        //     // DEBUG(packet->id);
        //     // DEBUG('\n');
        //     evokeCallbackFunction(packet);
        // } else if(Serial.available()) {
        //     int cnt = 0;
        //     while(Serial.available() && cnt < sizeof(Packet)) {
        //         packetBuffer[cnt] = Serial.read();
        //         cnt++;
        //     }
        //     Packet *packet = (Packet *)&packetBuffer;
        //     // DEBUG("Got unverified packet with ID ");
        //     // DEBUG(packet->id);
        //     // DEBUG('\n');
        //     evokeCallbackFunction(packet);
        // }
    }

    void packetAddFloat(Packet *packet, float value) {
        uint32_t rawData = * ( uint32_t * ) &value;
        packet->data[packet->len] = rawData & 0xFF;
        packet->data[packet->len + 1] = rawData >> 8 & 0xFF;
        packet->data[packet->len + 2] = rawData >> 16 & 0xFF;
        packet->data[packet->len + 3] = rawData >> 24 & 0xFF;
        packet->len += 4;
    }

    void packetAddUint32(Packet *packet, uint32_t value) {
        packet->data[packet->len] = value & 0xFF;
        packet->data[packet->len + 1] = value >> 8 & 0xFF;
        packet->data[packet->len + 2] = value >> 16 & 0xFF;
        packet->data[packet->len + 3] = value >> 24 & 0xFF;
        packet->len += 4;
    }

    void packetAddUint16(Packet *packet, uint16_t value) {
        packet->data[packet->len] = value & 0xFF;
        packet->data[packet->len + 1] = value >> 8 & 0xFF;
        packet->len += 2;
    }

    void packetAddUint8(Packet *packet, uint8_t value) {
        packet->data[packet->len] = value;
        packet->len++;
    }

    float packetGetFloat(Packet *packet, uint8_t index) {
        uint32_t rawData = packet->data[index+3];
        rawData <<= 8;
        rawData += packet->data[index+2];
        rawData <<= 8;
        rawData += packet->data[index+1];
        rawData <<= 8;
        rawData += packet->data[index];
        return * ( float * ) &rawData;
    }

    uint32_t packetGetUint32(Packet *packet, uint8_t index) {
        uint32_t rawData = packet->data[index+3];
        rawData <<= 8;
        rawData += packet->data[index+2];
        rawData <<= 8;
        rawData += packet->data[index+1];
        rawData <<= 8;
        rawData += packet->data[index];
        return rawData;
    }

    uint32_t packetGetUint8(Packet *packet, uint8_t index) {
        return packet->data[index];
    }

    /**
     * @brief Sends packet to both groundstations.
     * 
     * @param packet Packet to be sent.
     */
    int emitPacket(Packet *packet, uint8_t *buffer) {
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

        // Send over serial, but disable if in debug mode
        // for (int i = 0; i < packet->len; i++) {
        //     Serial.print(packet->data[i], HEX);
        // }
        // Serial.print('\n', HEX);

        //Serial.printf("p%d\n", packet->id);
        //TRANSMITTING CODE BELOW
        Serial.write(packet->id);
        Serial.write(packet->len);
        Serial.write(packet->timestamp, 4);
        Serial.write(packet->checksum, 2);
        Serial.write(packet->data, packet->len);
        Serial.write('\r');
        Serial.write('\n');
        Serial.write('\n');

        Serial2.write(packet->id);
        Serial2.write(packet->len);
        Serial2.write(packet->timestamp, 4);
        Serial2.write(packet->checksum, 2);
        Serial2.write(packet->data, packet->len);
        Serial2.write('\r');
        Serial2.write('\n');
        Serial2.write('\n');

        int numBytes = 1 + 1 + 4 + 2 + packet->len + 1;

        buffer[0] = packet->id;
        buffer[1] = packet->len;
        buffer[2] = packet->timestamp[0]; buffer[3] = packet->timestamp[1]; buffer[4] = packet->timestamp[2]; buffer[5] = packet->timestamp[3];
        buffer[6] = packet->checksum[0]; buffer[7] = packet->checksum[1];
        for (int i = 0; i < packet->len; i++) {
            buffer[8+i] = packet->data[i];
        }

        // Serial.write(packet->id);
        // Serial.write(packet->len);
        // Serial.write(packet->timestamp, 4);
        // Serial.write(packet->checksum, 2);
        // Serial.write(packet->data, packet->len);
        // Serial.write('\n');



        //Send over ethernet to both ground stations
        // Udp.beginPacket(groundStation1, port);
        // Udp.write(packet->id);
        // Udp.write(packet->len);
        // Udp.write(packet->timestamp, 4);
        // Udp.write(packet->checksum, 2);
        // Udp.write(packet->data, packet->len);
        // Udp.endPacket();

        // Udp.beginPacket(groundStation2, port);
        // Udp.write(packet->id);
        // Udp.write(packet->len);
        // Udp.write(packet->timestamp, 4);
        // Udp.write(packet->checksum, 2);
        // Udp.write(packet->data, packet->len);
        // Udp.endPacket();
        // Serial.printf("numBytes: %d\n", numBytes);
        return numBytes;
    }

    // void emitPacket(Packet *packet, uint8_t end) {
    //     DEBUG(end);
    //     DEBUG('\n');
    //     //add timestamp to struct
    //     uint32_t timestamp = millis();
    //     packet->timestamp[0] = timestamp & 0xFF;
    //     packet->timestamp[1] = (timestamp >> 8) & 0xFF;
    //     packet->timestamp[2] = (timestamp >> 16) & 0xFF;
    //     packet->timestamp[3] = (timestamp >> 24) & 0xFF;

    //     //calculate and append checksum to struct
    //     uint16_t checksum = computePacketChecksum(packet);
    //     packet->checksum[0] = checksum & 0xFF;
    //     packet->checksum[1] = checksum >> 8;

    //     // Udp.beginPacket(IPAddress(10, 0, 0, end), port);
    //     // Udp.write(packet->id);
    //     // Udp.write(packet->len);
    //     // Udp.write(packet->timestamp, 4);
    //     // Udp.write(packet->checksum, 2);
    //     // Udp.write(packet->data, packet->len);
    //     // Udp.endPacket();
    // }

    /**
     * @brief generates a 2 byte checksum from the information of a packet
     * 
     * @param data pointer to data array
     * @param len length of data array
     * @return uint16_t 
     */
    uint16_t computePacketChecksum(Packet *packet) {

        uint8_t sum1 = 0;
        uint8_t sum2 = 0;

        sum1 = sum1 + packet->id;
        sum2 = sum2 + sum1;
        sum1 = sum1 + packet->len;
        sum2 = sum2 + sum1;
        
        for (uint8_t index = 0; index < 4; index++) {
            sum1 = sum1 + packet->timestamp[index];
            sum2 = sum2 + sum1;
        }

        for (uint8_t index = 0; index < packet->len; index++) {
            sum1 = sum1 + packet->data[index];
            sum2 = sum2 + sum1;
        }
        return (((uint16_t)sum2) << 8) | (uint16_t) sum1;
    }
};