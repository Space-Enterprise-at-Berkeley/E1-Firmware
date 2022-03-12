#pragma once

#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <LittleFS.h>
#include "Comms.h"

namespace BlackBox {
    void init();
    void erase(Comms::Packet packet);
    void beginWrite(Comms::Packet packet);

    /**
     * @brief Prints data over Ethernet and Serial.
     * 
     */
    void getData(Comms::Packet packet);

    /**
     * @brief Registers a packet to be written to the blackbox as specified by its update period.
     * 
     * @param packetEmitter A struct containing the packet to be sent and the update period.
     */
    void registerEmitter(Comms::PacketEmitter packetEmitter);

    /**
     * @brief Write all packets registered by emitters as specified by update period.
     * 
     */
    void writePackets();
}