#pragma once

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <map>
#include <string>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDA, 0x02};
int port = 42069;
IPAddress ip(10, 0, 0, 42);

namespace Comms {
    struct Packet {
        uint8_t id;
        uint8_t len;
        uint8_t checksum[2];
        uint8_t data[256];
    };

    void initComms();

    typedef void (*commFunction)(Packet);

    /**
     * @brief Registers methods to be called when Comms receives a packet with a specific ID.
     * 
     * @param id The ID of the packet associated with a specific command.
     * @param function a pointer to a method that takes in a Packet struct.
     */
    void registerCallback(int id, commFunction function);

    uint32_t packetWaiting();
    void processPackets();

    void packetAddFloat(Packet *packet, float value);

    /**
     * @brief Sends packet data over ethernet and serial.
     * 
     * @param packet The packet in which the data is stored.
     */
    void emitPacket(Packet *packet);
};
