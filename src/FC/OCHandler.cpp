#include "OCHandler.h"

namespace OCHandler {

    PCA9539 ioOCExpander(0x74);

    void initOCHandler(uint8_t interruptPin) {
        //setup IO Expander pins
        ioOCExpander.pinMode(1, INPUT); //TODO check this implementation
        ioOCExpander.pinMode(2, INPUT);
        ioOCExpander.pinMode(3, INPUT);
        ioOCExpander.pinMode(4, INPUT);
        ioOCExpander.pinMode(5, INPUT);
        ioOCExpander.pinMode(6, INPUT);
        ioOCExpander.pinMode(7, INPUT);
        ioOCExpander.pinMode(8, INPUT);
        ioOCExpander.pinMode(9, INPUT);
        ioOCExpander.pinMode(10, INPUT);
        ioOCExpander.pinMode(11, INPUT);
        ioOCExpander.pinMode(12, INPUT);
        ioOCExpander.pinMode(13, INPUT);

        pinMode(interruptPin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(interruptPin), OCInterrupt, LOW);
    }

    Comms::Packet ocPacket = {.id = 11};
    void OCInterrupt() {
        for (int i = 0; i < 14; i++) {
            if (ioOCExpander.digitalRead(i)) {
                Comms::packetAddUint8(&ocPacket, i);
            }
        }
        Comms::emitPacket(&ocPacket);
    }
}
