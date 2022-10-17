#include "BlackBox.h"
    namespace BlackBox {
        int lastElement = 0;
        int page = 0;
        int pageAddress = 0;
        
        bool full = false;
        // W25Q w25q;
        SPIFlash bb(10);
        const int bufferSize = 20*(64 + 2);
        uint8_t buffer[bufferSize]; 
        Comms::Packet metadataPacket = {.id = 55};
        uint8_t first_byte;

        uint32_t updatePeriod = 1000 * 1000;

        /*******************************Public Methods******************************/

        /*****************************************************************************
        * BlackBox saves packets contiguously in memory on the W25Q chip for worst case
        * scenerios 
        * @brief initlizes W25Q Memory chip and clears buffer
        *****************************************************************************/

        void init(int FLASH_SS) {
            // w25q.init(FLASH_SS);
            bb.initialize();
            clearBuffer();
            if (bb.readByte(0) == 0xFF) {
                Comms::registerEmitter(writeToBuffer);
                Serial.println("RECORDING");
            } else {
                Serial.println("NOT RECORDING");
            }

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

            memcpy(buffer, &packet, 8 + packet.len);

            writeBufferToBB(8 + packet.len, packet.id);

            // Serial.printf("adding packet to buffer, %d\n", lastElement);
            // if(lastElement + packet.len + 8 >= bufferSize) {
            //     Serial.printf("Writing buffer to BB\n");
            //     writeBufferToBB();
            //     clearBuffer();
            //     lastElement = 0;
            // } 
            // memcpy(buffer + lastElement, &packet, packet.len + 8);
            // lastElement += packet.len + 8;
            
        }

        int sendMetadataPacket() {
            metadataPacket.len = 0;
            Comms::packetAddUint32(&metadataPacket, (int) pageAddress / 1000);
            Comms::packetAddUint8(&metadataPacket, 0);
            Comms::emitPacket(&metadataPacket);

            return updatePeriod;
        }
        /*******************************Private Methods******************************/

        /*****************************************************************************
        * Function: WriteBufferToBB
        *
        * Description @brief if W25Q is not full: opens reader, writes buffer
        *  into chip and closes reader   
        *****************************************************************************/
        void writeBufferToBB(int len, int packetid) {
            DEBUG("writing out buffer to black box\n");
            if (full) {
                return;
            }
            Serial.printf("got packet with total len %d\n", len);

            if ((pageAddress + len > 16000000)) {
                return;
            } else {
                bb.writeBytes(pageAddress, buffer, len);
                pageAddress += len;
            }


            // w25q.initStreamWrite(page, pageAddress);
            // // Serial.printf("got packet with total len %d\n", len);
            // for (int i = 0; i < len; i++) {
            //     uint8_t elem = buffer[i];
            //     if (elem == 0xFF) {
            //         Serial.printf("0xff found with packetid %d\n", packetid);
            //     }
            //     w25q.streamWrite(elem);
            //     pageAddress++;
            //     if (pageAddress >= w25q.pageSize) {
            //         // Serial.printf("BEFORE: changed page to page %d, pageAddress = %d\n", page, pageAddress);
            //         pageAddress = 0;
            //         if (page > 65536) {
            //             full = true;
            //         }
            //         page++;
            //         w25q.initStreamWrite(page, pageAddress);
            //         // Serial.printf("AFTER:  changed page to page %d, pageAddress = %d\n", page, pageAddress);
            //     }
            // }
            // w25q.closeStreamWrite();
        }
        /*****************************************************************************
        * Function: clearBuffer
        * 
        * Description: @brief attempts to clear the buffers memory 
        *****************************************************************************/
        void clearBuffer() {
            memset(buffer, 0, sizeof(buffer));
        }

        void dump(int maxDumpPages) {

            if (maxDumpPages == -1) maxDumpPages = 65536;
            int maxPages = maxDumpPages < 65536 ? maxDumpPages : 65536;


            int ptr = 0;
            while (ptr < maxPages*256) {
                #ifdef DUMP_BINARY
                Serial.write(bb.readByte(ptr));
                #endif
                #ifdef DUMP_HEX
                Serial.printf("%x ", bb.readByte(ptr));
                #endif
                ptr++;
                #ifdef DUMP_HEX 
                if (ptr % 256 == 0) Serial.printf("\n\n");
                #endif
            }
        }


        /*******************************Testing Methods******************************/
        /*****************************************************************************
        * Function: resetAll
        * 
        * Description: @brief clears buffer and ereases all chip data
        *****************************************************************************/
        void resetAll() {
            clearBuffer();
            // w25q.chipErase();
            bb.chipErase();
            while (bb.busy()) {}
        }


    }
