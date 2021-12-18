#pragma once

#include <Arduino.h>

namespace Comms {
    uint32_t packetWaiting();
    void processPackets();
};
