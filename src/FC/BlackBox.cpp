#include "BlackBox.h"
    namespace BlackBox {
        uint8_t lastElement = 0;
        uint8_t page = 0;
        byte pageAddress = 0;
        bool full = false;
        W25Q w25q;
        const int bufferSize = 20*(64 + 2);
        volatile uint8_t buffer[bufferSize]; 

        /*******************************Public Methods******************************/

        /*****************************************************************************
        * BlackBox saves packets contiguously in memory on the W25Q chip for worst case
        * scenerios 
        * @brief initlizes W25Q Memory chip and clears buffer
        *****************************************************************************/

        void init(int FLASH_SS) {
            w25q.init(FLASH_SS);
            clearBuffer();
            Comms::registerEmitter(writeToBuffer);
        }

        /*****************************************************************************
        * Function: WriteToBuffer
        *
        * Returns: @returns uint32_t 
        * -> updatePeriod
        * 
        * Parameters @param packet
        * 
        * Description: @brief writes the packet to buffer, once the buffer is full 
        * buffer is writen onto W25Q. Amortized O(n).
        *****************************************************************************/
        void writeToBuffer(Comms::Packet packet, uint8_t ip) {
            if(lastElement + packet.len + 8 >= bufferSize) {
                writeBufferToBB();
                clearBuffer();
                lastElement = 0;
            }
                memcpy(buffer + lastElement, &packet, packet.len + 8);
                lastElement += packet.len + 8;
        }
        /*******************************Private Methods******************************/

        /*****************************************************************************
        * Function: WriteBufferToBB
        *
        * Description @brief if W25Q is not full: opens reader, writes buffer
        *  into chip and closes reader   
        *****************************************************************************/
        void writeBufferToBB() {
            if (full) {
                return;
            }
            w25q.initStreamWrite(page, pageAddress);
            for (uint8_t elem: buffer) {
                w25q.streamWrite(elem);
                pageAddress++;
                if (pageAddress > w25q.pageSize - 1) {
                    pageAddress = pageAddress % 256;
                    if (page > 65536) {
                        full = true;
                    }
                    page++;
                    w25q.initStreamWrite(page, pageAddress);
                }
            }
            w25q.closeStreamWrite();
        }
        /*****************************************************************************
        * Function: clearBuffer
        * 
        * Description: @brief attempts to clear the buffers memory 
        *****************************************************************************/
        void clearBuffer() {
            memset(buffer, 0, sizeof(buffer));
        }


        /*******************************Testing Methods******************************/
        /*****************************************************************************
        * Function: resetAll
        * 
        * Description: @brief clears buffer and ereases all chip data
        *****************************************************************************/
        void resetAll() {
            clearBuffer();
            w25q.chipErase();
        }
    }
