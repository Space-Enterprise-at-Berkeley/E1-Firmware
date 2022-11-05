#include "BlackBox.h"

namespace BlackBox {
    LittleFS_QSPIFlash blackBox;
    File dataFile;
    const char* path = "data";


    void init() {
        Comms::registerEmitter(writePacket);
        uint8_t success = blackBox.begin();
        if (!success) {
            Serial.println("error starting blackbox");
        }
        dataFile = blackBox.open(path, FILE_WRITE);
        Serial.println("blackbox init success");
    }

    void writePacket(Comms::Packet packet, uint8_t ip) {
        uint16_t len = 8 + packet.len;
        dataFile.write(&packet, len);
        // dataFile.close();
    }

    Comms::Packet getData(uint32_t byteAddress) {
        dataFile.close();
        Comms::Packet packet;
        dataFile = blackBox.open(path, FILE_READ);
        dataFile.seek(byteAddress);
        dataFile.readBytes((char *)&packet, sizeof(Comms::Packet));
        return packet;
    }

    void erase() {
        blackBox.quickFormat();
        Serial.println("blackbox erased");
    }

    void dumpData() {
        dataFile = blackBox.open(path);
        while(dataFile.available()) {
            Serial.print(dataFile.read());
        }
    }
}