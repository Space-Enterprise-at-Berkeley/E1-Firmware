#include "BlackBox.h"

namespace BlackBox {

    const char* filePath = "data.txt";

    LittleFS_QSPIFlash blackBox;

    EthernetUDP Udp;

    const int bufferSize = 2048;
    int bufferIndex = 0;
    uint8_t buffer[bufferSize] = {0};

    bool writeEnabled = false;

    Comms::Packet dataDumpPacket = {.id = 54};

    void init() {
        Comms::registerPacketSubscriber(&writePacket);
        Comms::registerCallback(153, &getData);
        Comms::registerCallback(154, &beginWrite);
        Comms::registerCallback(155, &erase);

        blackBox.begin();

        //check for previous data, else initialize data
        bool hasData = blackBox.exists(filePath);

        if(!hasData) {
            DEBUG("BLACKBOX: NO PREVIOUS DATA DETECTED");
            DEBUG("\n");
        } else {
            DEBUG("BLACKBOX: PREVIOUS DATA DETECTED");
            DEBUG("\n");
        }
    }

    void beginWrite(Comms::Packet packet) {
        DEBUG("BLACKBOX: WRITE ENABLED");
        DEBUG(" WITH USED SPACE: ");
        DEBUG(blackBox.usedSize());
        DEBUG("\n");
        writeEnabled = true;
    }

    void writePacket(Comms::Packet packet) {
        if (writeEnabled && blackBox.usedSize() + 50 <= blackBox.totalSize()) {

            //check if buffer full
            if (bufferIndex + 8 + packet.len > bufferSize) {
                writeBuffer();
            }

            //write metadata
            buffer[bufferIndex] = packet.id;
            buffer[bufferIndex + 1] = packet.len;
            buffer[bufferIndex + 2] = packet.timestamp[0];
            buffer[bufferIndex + 3] = packet.timestamp[1];
            buffer[bufferIndex + 4] = packet.timestamp[2];
            buffer[bufferIndex + 5] = packet.timestamp[3];
            buffer[bufferIndex + 6] = packet.checksum[0];
            buffer[bufferIndex + 7] = packet.checksum[1];

            bufferIndex += 8;

            for (int i = 0; i < packet.len; i++) {
                buffer[bufferIndex] = packet.data[i];
                bufferIndex += 1;
            }
        }
    }

    /**
     * @brief writes and clears the packet buffer
     * 
     */
    void writeBuffer() {
        File dataFile = blackBox.open(filePath, FILE_WRITE);
        dataFile.write(buffer, bufferIndex + 1);
        dataFile.close();
        //reset buffer
        std::fill_n(buffer, bufferSize, 0);
        bufferIndex = 0;
    }

    void erase(Comms::Packet packet) {
        blackBox.remove(filePath);
        writeEnabled = false;
        DEBUG("BLACKBOX: DATA ERASED");
        DEBUG("\n");
    }

    void getData(Comms::Packet packet) {
        DEBUG("BLACKBOX: IN GET DATA");
        DEBUG("\n");
        if (blackBox.exists(filePath)) {

            uint8_t readBuffer[256] = {0};

            DEBUG("BLACKBOX: EMITTING DATA DUMP PACKET");
            DEBUG("\n");
            // signal beginning of data dump
            dataDumpPacket.len = 0;
            Comms::packetAddUint8(&dataDumpPacket, 0);
            Comms::emitPacket(&dataDumpPacket);

            File dataFile = blackBox.open(filePath, FILE_READ);
            DEBUG("BLACKBOX: READING FROM FILE");
            DEBUG("\n");
            while(dataFile.read(readBuffer, 256) > 0) {
                for (int i = 0; i < 256; i++) {
                // DEBUG("BLACKBOX: WRITING OVER ETHERNET");
                // DEBUG("\n");
                    Serial.write(readBuffer[i]);
                    Serial.flush();
                    // Udp.write(readBuffer[i]);
                }
            }
            
            dataFile.close();
            DEBUG("BLACKBOX: DUMP END");
            DEBUG("\n");
            writeEnabled = false;

            // signal ending of data dump
            dataDumpPacket.len = 0;
            Comms::packetAddUint8(&dataDumpPacket, 1);
            Comms::emitPacket(&dataDumpPacket);
        } else {
            DEBUG("BLACKBOX: FILE DOESN'T EXIST");
            DEBUG("\n");
        }
    }
}