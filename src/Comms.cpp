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
        callbackMap.at(packet.id)(packet);
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
        //Send over serial
        for (int i = 0; i < packet->len; i++) {
            Serial.println(packet->data[i]);
        }
        Serial.println("");

        //Send over ethernet
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(packet->data, packet->len);
        Udp.endPacket();
    }
};
