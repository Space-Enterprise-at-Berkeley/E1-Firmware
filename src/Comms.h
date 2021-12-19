#pragma once

#include <Arduino.h>

namespace Comms {
    struct Packet {
        uint8_t id;
        uint8_t data[256];
        uint8_t len;
    };

    void initComms();

    uint32_t packetWaiting();
    void processPackets();

    void packetAddFloat(Packet *packet, float value);
    void emitPacket(Packet *packet);
};
