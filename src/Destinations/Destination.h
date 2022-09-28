#pragma once

#include "Comms.h"

class Destination {

    public:
    void emitPacket(Comms::Packet packet);
    bool available();
    void read(char* buffer);

};