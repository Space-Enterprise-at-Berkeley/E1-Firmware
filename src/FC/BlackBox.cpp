#include "BlackBox.h"

namespace BlackBox {
    LittleFS_QSPIFlash blackBox;
    File dataFile;
    const char* path = "data";


    void init() {
        Comms::registerEmitter(writePacket);
        blackBox.begin();
    }

    void writePacket(Comms::Packet packet, uint8_t ip) {
        dataFile = blackBox.open(path, FILE_WRITE);
        dataFile.write(&packet, sizeof(Comms::Packet));
    }

    Comms::Packet getData(uint32_t byteAddress) {
        Comms::Packet packet;
        dataFile = blackBox.open(path, FILE_READ);
        dataFile.seek(byteAddress);
        dataFile.readBytes((char *)&packet, sizeof(Comms::Packet));
        return packet;
    }
}