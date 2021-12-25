#pragma once

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <map>
#include <string>

//TODO replace with actual values
byte mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int port = 0;
IPAddress ip(192, 168, 1, 177);

namespace Comms {
    struct Packet {
        uint8_t id;
        uint8_t data[256];
        uint8_t len;
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
