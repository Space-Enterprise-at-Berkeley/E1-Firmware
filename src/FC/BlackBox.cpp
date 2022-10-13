#include "BlackBox.h"
    namespace BlackBox {
        const uint32_t updatePeriod = 100 * 1000; //TODO figure out update period
        uint8_t lastElement = 0;
        uint8_t page = 0;
        byte pageAddress = 0;
        bool Full = false;
        W25Q w25q;

        // buffer has 20 packets, each packet has 66 uint8_t, TODO, confirm packet size;
        int bufferSize = 20*(sizeof(Comms::Packet));
        uint8_t buffer[bufferSize]; //TODO should mem be allocated here or in init?

        /*******************************Public Methods******************************/

        /*****************************************************************************
        * BlackBox saves packets contiguously in memory on the W25Q chip for worst case
        * scenerios 
        * @brief initlizes W25Q Memory chip and clears buffer
        *****************************************************************************/

        void init() {
            w25q.init()
            clearBuffer()
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
        uint32_t writeToBuffer(Comms::Packet packet) {
            if(lastElement >= bufferSize) {
                writeBufferToBB()
                lastElement = 0;
            }
            //memecpy(&buffer,&packet, 66) TODO, use memcpy for nicer code
            for(uint8_t uInt : packet) {
                buffer[lastElement] = uInt;
                lastElement++;
            }
            
            return updatePeriod;

        }
        /*******************************Private Methods******************************/
        /*****************************************************************************
        * Function: WriteBufferToBB
        *
        * Description @brief if W25Q is not full: opens reader, writes buffer
        *  into chip and closes reader   
        *****************************************************************************/
        void writeBufferToBB() {
            if (Full) {
                return;
            }
            w25q.initStreamWrite(page, pageAddress)
            for (uint8_t uInt: buffer) {
                w25q.streamWrite(uInt);//TODO, how to write all bytes in the package
                pageAddress++;
                if (pageAddress > 256 ) {
                    pageAddress = pageAddress % 256;
                    if (page > w25q.numPages - 1) {
                        Full = true;
                    }
                    page++;
                }
            }
            w25q.closeStreamWrite()
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
            fc.chipErase();
        }
    }
    