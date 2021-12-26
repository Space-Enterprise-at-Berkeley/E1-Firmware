#include <Comms.h>

namespace Comms {

    std::map<int, commFunction> callbackMap;
    EthernetUDP Udp;
    char packetBuffer[sizeof(Packet)];

    void initComms() {
        Ethernet.begin((uint8_t *)mac, ip);
        Udp.begin(port);
    }

    void registerCallback(int id, commFunction function) {
        callbackMap.insert(std::pair<int, commFunction>(id, function));
    }

    uint32_t packetWaiting() {
        return Udp.parsePacket();
    }

    void processPackets() {
        Udp.read(packetBuffer, sizeof(Packet));

        Packet *packet = (Packet *)&packetBuffer;

        //call callback function
        uint16_t checksum = *(uint16_t *)&packet->checksum;
        if (checksum == computePacketChecksum(packet)) {
            callbackMap.at(packet->id)(*packet);
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

    void emitPacket(Packet *packet) {
        //calculate and append checksum to struct
        uint16_t checksum = computePacketChecksum(packet);
        uint8_t *ptr = (uint8_t *)&checksum;
        packet->checksum[0] = ptr[0];
        packet->checksum[1] = ptr[1];

        //Send over serial
        Serial.write(packet->id);
        Serial.write(packet->len);
        Serial.write(packet->checksum, 2);
        Serial.write(packet->data, packet->len);
        Serial.write('\n');

        //Send over ethernet to both ground stations
        Udp.beginPacket(groundStation1, port);
        Udp.write(packet->id);
        Udp.write(packet->len);
        Udp.write(packet->checksum, 2);
        Udp.write(packet->data, packet->len);
        Udp.endPacket();

        Udp.beginPacket(groundStation2, port);
        Udp.write(packet->id);
        Udp.write(packet->len);
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

        uint16_t sum1 = packet->id;
        uint16_t sum2 = (packet-> id + packet-> len) % 256;

        for (int index = 0; index < packet->len; index++) {
            if (packet->data[index] > 0) {
                sum1 = (sum1 + packet->data[index]) % 256;
                sum2 = (sum2 + sum1) % 256;
            }
        }
        return (sum2 << 8) | sum1;
    }
};
