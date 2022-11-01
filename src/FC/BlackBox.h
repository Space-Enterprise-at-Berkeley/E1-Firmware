#pragma once

#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <LittleFS.h>
#include "Comms.h"

namespace BlackBox {

    void init();
    void writePacket(Comms::Packet packet, uint8_t ip);
    Comms::Packet getData(uint32_t byteAddress);
}