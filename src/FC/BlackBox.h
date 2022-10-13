#pragma once

#include <W25Q.h>
#include <Comms.h>

namespace BlackBox {

        void init();
        void writeToBuffer();  
        void clearBuffer();   
        void writeBufferToBB();
}