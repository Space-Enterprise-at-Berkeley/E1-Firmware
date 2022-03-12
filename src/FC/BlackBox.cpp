#include "BlackBox.h"

namespace BlackBox {

    const char* filePath = "data.txt";

    LittleFS_QSPIFlash blackBox;

    EthernetUDP Udp;

    int bufferIndex = 0;
    uint8_t buffer[256] = {0};

    std::vector<Comms::PacketEmitter> emitters;

    bool writeEnabled = false;

    Comms::Packet dataDumpPacket = {.id = 54};
    
    /**
     * @brief writes and clears the packet buffer
     * 
     */
    void writeBuffer() {
        File dataFile = blackBox.open(filePath, FILE_WRITE);
        dataFile.write(buffer, 256);
        dataFile.close();
        //reset buffer
        std::fill_n(buffer, 256, 0);
        bufferIndex = 0;
    }
    void init() {
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
        writeEnabled = true;
    }

    void writePacket(Comms::Packet packet) {
        if (writeEnabled && blackBox.usedSize() + 50 <= blackBox.totalSize()) {

            //check if buffer full
            if (bufferIndex + 8 + packet.len > 256) {
                writeBuffer();
            }

            //write metadata
            buffer[bufferIndex] = packet.id;
            buffer[bufferIndex + 1] = packet.len;
            buffer[bufferIndex + 2] = packet.timestamp[0];
            buffer[bufferIndex + 3] = packet.timestamp[1];
            buffer[bufferIndex + 4] = packet.timestamp[2];
            buffer[bufferIndex + 5] = packet.timestamp[3];
            buffer[bufferIndex + 6] = packet.checksum[1];
            buffer[bufferIndex + 7] = packet.checksum[2];

            bufferIndex += 8;

            for (int i = 0; i < packet.len; i++) {
                buffer[bufferIndex] = packet.data[i];
                bufferIndex += 1;
            }
        }
    }

    void erase(Comms::Packet packet) {
        blackBox.remove(filePath);
        writeEnabled = false;
        DEBUG("BLACKBOX: DATA ERASED");
        DEBUG("\n");
    }

    void getData(Comms::Packet packet) {
        if (blackBox.exists(filePath)) {

            uint8_t readBuffer[256] = {0};

            // signal beginning of data dump
            dataDumpPacket.len = 0;
            Comms::packetAddUint8(&dataDumpPacket, 0);
            Comms::emitPacket(&dataDumpPacket);

            Udp.beginPacket(Comms::groundStation1, Comms::port);

            File dataFile = blackBox.open(filePath, FILE_READ);
            while(dataFile.read(readBuffer, 256) > 0) {
                for (int i = 0; i < 256; i++) {
                    Serial.write(readBuffer[i]);
                    Udp.write(readBuffer[i]);
                }
            }
            
            Udp.endPacket();
            dataFile.close();
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


    void writePackets() {
        uint32_t currTime = micros();
        for (Comms::PacketEmitter emitter: emitters) {
            if (currTime - emitter.lastTime > emitter.updatePeriod) {
                writePacket(*emitter.packet);
                emitter.lastTime = currTime;
            }
        }
    }

    void registerEmitter(Comms::PacketEmitter packetEmitter) {
        emitters.push_back(packetEmitter);
    }
}