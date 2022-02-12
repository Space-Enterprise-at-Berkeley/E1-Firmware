#include "BlackBox.h"
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

namespace BlackBox {

    W25Q blackBox;

    EthernetUDP Udp;

    int bufferIndex = 0;
    int pageIndex;
    uint8_t buffer[256] = {0};

    void init() {
        Comms::registerPacketSubscriber(&writePacket);
        Comms::registerCallback(153, &getData);

        blackBox.init(10);

        //check for previous data, else initialize data
        bool hasData = blackBox.read(0, 0);

        if (hasData) {
            pageIndex = blackBox.read(0, 1);
        } else {
            pageIndex = 0;
            blackBox.write(0, 0, 1);
        }
    }

    void writePacket(Comms::Packet packet) {
        //check if buffer full
        if (bufferIndex + 8 + packet.len > blackBox.pageSize) {
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

    /**
     * @brief writes and clears the packet buffer
     * 
     */
    void writeBuffer() {
        for (int i = 0; i < 256; i++) {
            blackBox.write(pageIndex, i, buffer[i]);
        }
        //reset buffer
        std::fill_n(buffer, 256, 0);
        bufferIndex = 0;
        pageIndex += 1;
    }

    void erase() {
        blackBox.chipErase();
        blackBox.write(0, 0, 0);
    }

    void getData(Comms::Packet packet) {
        int size = pageIndex * 256;
        blackBox.initStreamRead(0, 0);

        Udp.beginPacket(Comms::groundStation1, Comms::port);
        for (int i = 0; i < size; i++) {
            uint8_t data = blackBox.streamRead();
            Serial.write(data);
            Udp.write(data);
        }
        Udp.endPacket();
        blackBox.closeStreamRead();
    }
}