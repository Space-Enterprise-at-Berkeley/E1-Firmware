#pragma once

#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <LittleFS.h>
#include "Comms.h"

namespace BlackBox {
    void init();
    void writePacket(Comms::Packet packet);
    bool hasData();
    void writeBuffer();
    void erase(Comms::Packet packet);
    void beginWrite(Comms::Packet packet);

    /**
     * @brief Prints data over Ethernet and Serial.
     * 
     */
    void getData(Comms::Packet packet);
}