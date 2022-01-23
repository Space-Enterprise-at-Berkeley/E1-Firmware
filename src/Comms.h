#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <map>
#include <Common.h>     

namespace Comms {
    const int port = 42069;

    const char * const ssid = "eureka_cart";
    const char * const password = "Up_Dog69";
    // const char * const ssid = "HUAWEI P30 lite";
    // const char * const password = "Hihihi123";


    const IPAddress local_IP(10, 0, 0, IP_ADDRESS_END);
    const IPAddress gateway(10, 0, 0, 1);

    const IPAddress subnet(255, 255, 255, 0);

    const IPAddress groundStation1(10, 0, 0, 69);
    const IPAddress groundStation2(10, 0, 0, 70);
    const IPAddress groundStation3(10, 0, 0, 31);
    const IPAddress groundStation4(10, 0, 0, 32);

    struct Packet {
        uint8_t id;
        uint8_t len;
        uint8_t timestamp[4];
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
    void registerCallback(uint8_t id, commFunction function);

    void processWaitingPackets();

    void packetAddFloat(Packet *packet, float value);
    void packetAddUint8(Packet *packet, uint8_t value);

    float packetGetFloat(Packet *packet, uint8_t index);

    /**
     * @brief Sends packet data over ethernet and serial.
     * 
     * @param packet The packet in which the data is stored.
     */
    void emitPacket(Packet *packet);

    uint16_t computePacketChecksum(Packet *packet);
};