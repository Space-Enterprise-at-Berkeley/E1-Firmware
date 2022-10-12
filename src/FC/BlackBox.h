#pragma once

#include <W25Q.h>
#include <Comms.h>

namespace BlackBox {

    public:
        void init()
        uint32_t WriteToBuffer();
    private:
        const uint32_t updatePeriod;
        const uint8_t bufferSize;
        uint8_t lastElement;
        uint8_t page;
        bool Full;
        byte pageAddress;
        volatile Comms::Packet buffer[];
        void writeBufferToBB()
        void clearBuffer()
}