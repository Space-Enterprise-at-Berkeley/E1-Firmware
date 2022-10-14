#pragma once

#include <W25Q.h>
#include <Comms.h>

namespace BlackBox {

        void init(int FLASH_SS);
        void writeToBuffer(Comms::Packet packet, uint8_t ip);  
        void clearBuffer();   
        void writeBufferToBB(int len, int packetid);
        int sendMetadataPacket();
        void resetAll();
        void dump(int maxDumpPages);
}