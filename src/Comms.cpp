#include <Comms.h>

namespace Comms {

    std::map<uint8_t, commFunction> callbackMap;
    EthernetUDP Udp;
    char packetBuffer[sizeof(Packet)];

    void initComms() {
        Ethernet.begin((uint8_t *)mac, ip);
        Udp.begin(port);
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
            DEBUG("Packet with ID ");
            DEBUG(packet->id);
            DEBUG(" has correct checksum!\n");
            //try to access function, checking for out of range exception
            if(callbackMap.count(packet->id)) {
                callbackMap.at(packet->id)(*packet);
            } else {
                DEBUG("ID ");
                DEBUG(packet->id);
                DEBUG(" does not have a registered callback function.\n");
            }
        } else {
            DEBUG("Packet with ID ");
            DEBUG(packet->id);
            DEBUG(" does not have correct checksum!\n");
        }
    }

    void processWaitingPackets() {
        if(Udp.parsePacket()) {
            if(Udp.remotePort() != port) return; // make sure this packet is for the right port
            if(!(Udp.remoteIP() == groundStation1) && !(Udp.remoteIP() == groundStation2)) return; // make sure this packet is from a ground station computer
            Udp.read(packetBuffer, sizeof(Packet));

            Packet *packet = (Packet *)&packetBuffer;
            DEBUG("Got unverified packet with ID ");
            DEBUG(packet->id);
            DEBUG('\n');
            evokeCallbackFunction(packet);
        } else if(Serial.available()) {
            int cnt = 0;
            while(Serial.available() && cnt < sizeof(Packet)) {
                packetBuffer[cnt] = Serial.read();
                cnt++;
            }
            Packet *packet = (Packet *)&packetBuffer;
            DEBUG("Got unverified packet with ID ");
            DEBUG(packet->id);
            DEBUG('\n');
            evokeCallbackFunction(packet);
        }
    }

    void packetAddFloat(Packet *packet, float value) {
        uint32_t rawData = * ( uint32_t * ) &value;
        packet->data[packet->len] = rawData & 0xFF;
        packet->data[packet->len + 1] = rawData >> 8 & 0xFF;
        packet->data[packet->len + 2] = rawData >> 16 & 0xFF;
        packet->data[packet->len + 3] = rawData >> 24 & 0xFF;
        packet->len += 4;
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

    void emitPacket(Packet *packet) {
        //calculate and append checksum to struct
        uint16_t checksum = computePacketChecksum(packet);
        packet->checksum[0] = checksum & 0xFF;
        packet->checksum[1] = checksum >> 8;

        //add timestamp to struct
       uint32_t timestamp = micros();
       packet->timestamp[0] = timestamp & 0xFF;
       packet->timestamp[1] = (timestamp >> 8) & 0xFF;
       packet->timestamp[2] = (timestamp >> 16) & 0xFF;
       packet->timestamp[3] = (timestamp >> 24) & 0xFF;

        // Send over serial, but disable if in debug mode
        #ifndef DEBUG_MODE
        Serial.write(packet->id);
        Serial.write(packet->len);
        Serial.write(packet->timestamp, 4);
        Serial.write(packet->checksum, 2);
        Serial.write(packet->data, packet->len);
        Serial.write('\n');
        #endif

        //Send over ethernet to both ground stations
        Udp.beginPacket(groundStation1, port);
        Udp.write(packet->id);
        Udp.write(packet->len);
        Udp.write(packet->timestamp, 4);
        Udp.write(packet->checksum, 2);
        Udp.write(packet->data, packet->len);
        Udp.endPacket();

        Udp.beginPacket(groundStation2, port);
        Udp.write(packet->id);
        Udp.write(packet->len);
        Udp.write(packet->timestamp, 4);
        Udp.write(packet->checksum, 2);
        Udp.write(packet->data, packet->len);
        Udp.endPacket();
    }

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
            sum1 = sum1 + packet->checksum[index];
            sum2 = sum2 + sum1;
        }

        for (uint8_t index = 0; index < packet->len; index++) {
            sum1 = sum1 + packet->data[index];
            sum2 = sum2 + sum1;
        }
        return (((uint16_t)sum2) << 8) | (uint16_t) sum1;
    }
};
