#include <Comms.h>

namespace Comms {

    std::map<int, commFunction> callbackMap;
    EthernetUDP Udp;
    char packetBuffer[sizeof(Packet)];

    void initComms() {
        Ethernet.begin(mac, ip);
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

        Packet packet = *(struct Packet *)&packetBuffer;

        //call callback function
        uint16_t checksum = *(uint16_t *)&packet.checksum;
        if (checksum == fletcher16(packet.data, packet.len)) {
            callbackMap.at(packet.id)(packet);
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
        //generate checksum from data and add it to the packet
        uint16_t checksum = fletcher16(packet->data, packet->len);
        uint8_t *ptr = (uint8_t *)&checksum;
        packet->checksum[0] = ptr[0];
        packet->checksum[1] = ptr[1];

        //Send over serial
        Serial.write(packet->id);
        Serial.write(packet->len);
        Serial.write(packet->checksum, 2);
        Serial.write(packet->data, packet->len);
        Serial.write("\n");

        //Send over ethernet
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(packet->id);
        Udp.write(packet->len);
        Udp.write(packet->checksum, 2);
        Udp.write(packet->data, packet->len);
        Udp.endPacket();
    }

    /**
     * @brief generates a 2 byte checksum
     * 
     * @param data pointer to data array
     * @param len length of data array
     * @return uint16_t 
     */
    uint16_t fletcher16(uint8_t *data, int len) {

        uint16_t sum1 = 0;
        uint16_t sum2 = 0;

        for (int index=0; index<len; index++) {
            if (data[index] > 0) {
            sum1 = (sum1 + data[index]) % 255;
            sum2 = (sum2 + sum1) % 255;
            }
        }
        return (sum2 << 8) | sum1;
    }
};
