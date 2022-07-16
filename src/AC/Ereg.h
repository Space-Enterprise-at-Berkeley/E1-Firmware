#pragma once

#include "Arduino.h"
#include "Comms.h"

namespace EReg {

    uint32_t samplePeriod = 12.5 * 1000; // 80 Hz

    void initEReg();
    void zero(Comms::Packet tmp, uint8_t ip);
    void setPressureSetpoint(Comms::Packet tmp, uint8_t ip);
    void regulation(Comms::Packet tmp, uint8_t ip);
    void flow(Comms::Packet tmp, uint8_t ip);
    void fullOpen(Comms::Packet tmp, uint8_t ip);

    uint32_t sampleEregReadings();
    void sendToEReg(Comms::Packet packet);
    uint32_t receiveFromEreg();
}