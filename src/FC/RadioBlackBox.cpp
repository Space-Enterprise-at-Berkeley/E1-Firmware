#include "RadioBlackBox.h"

namespace RadioBlackBox {

    void init() {
        DEBUG("Initializing radio black box\n");
        DEBUG_FLUSH();
        // Comms::registerCallback(29, beginWrite); // Begin writing to flash chip when Flight Mode packet received by FC
    }

    void beginWrite() {
        // send packet to radio to start writing. id 200, send 1
        Comms::Packet radioBBPacket = {.id = 200};
        Comms::packetAddUint8(&radioBBPacket, 1);
        Comms::emitPacket(&radioBBPacket, &RADIO_SERIAL, "\r\n\n", 3);
        DEBUG("Begin write sent to radio\n");
        DEBUG_FLUSH();
    }


}