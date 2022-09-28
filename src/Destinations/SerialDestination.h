#pragma once

#include "Destination.h"

class SerialDestination: public Destination {

    public:
    SerialDestination(HardwareSerial *serial, long baud);
    void emitPacket(Comms::Packet *packet);
    bool available();
    void read(char* buffer);

    private:
    HardwareSerial *serial_;
};