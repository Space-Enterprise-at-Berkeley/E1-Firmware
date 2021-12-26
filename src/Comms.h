#pragma once

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <map>

namespace Comms {
    //https://github.com/sstaub/TeensyID/issues/3
    uint32_t m1 = HW_OCOTP_MAC1;
    uint32_t m2 = HW_OCOTP_MAC0;
    const byte mac[] = {
        m1 >> 8, m1 >> 0, m2 >> 24, m2 >> 16, m2 >> 8, m2 >> 0
    };
    const int port = 42069;
    const IPAddress ip(10, 0, 0, 42);
    const IPAddress groundStation1(10, 0, 0, 69);
    const IPAddress groundStation2(10, 0, 0, 70);

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
