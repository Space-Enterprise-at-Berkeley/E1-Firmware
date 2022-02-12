#pragma once

#include "W25Q.h"

#include "Comms.h"

namespace BlackBox {
    void init();
    void writePacket(Comms::Packet packet);
    bool hasData();
    void erase();
    void writeBuffer();

    /**
     * @brief Prints data over Ethernet and Serial.
     * 
     */
    void getData(Comms::Packet packet);
}