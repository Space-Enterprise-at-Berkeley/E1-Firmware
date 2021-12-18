#include <Comms.h>

namespace Comms {
    void initComms() {
    }

    uint32_t packetWaiting() {
        return 0; // return 1 if packets waiting
    }

    void processPackets() {
        // process all waiting packets
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
        Serial.print(packet->data[0]);
        Serial.print(packet->data[1]);
        Serial.print(packet->data[2]);
        Serial.println(packet->data[3]);
    }
};
