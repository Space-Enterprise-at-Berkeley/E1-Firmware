#pragma once

#include "Comms.h"

namespace Camera {
    const uint8_t startArray[5] = {0x55, 0x01, 0x03, 0x01, 0xaa};

    void init();
    void startCamera(Comms::Packet packet);
}
