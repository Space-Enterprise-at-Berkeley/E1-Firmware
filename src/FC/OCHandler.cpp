#include "OCHandler.h"

namespace OCHandler {

    PCA9539 ioOCExpander(0x24);

    void initOCHandler(uint8_t interruptPin) {
        //setup IO Expander pins
        //TODO check this implementation
        ioOCExpander.pinMode(1, INPUT); // Chute1
        ioOCExpander.pinMode(2, INPUT); // Chute2
        ioOCExpander.pinMode(3, INPUT); // Amp
        ioOCExpander.pinMode(4, INPUT); // Cam1
        ioOCExpander.pinMode(5, INPUT); // Cam2
        ioOCExpander.pinMode(6, INPUT); // Radio
        ioOCExpander.pinMode(7, INPUT); // Valve1
        ioOCExpander.pinMode(8, INPUT); // Valve2
        ioOCExpander.pinMode(9, INPUT); // Valve3
        ioOCExpander.pinMode(10, INPUT); // Valve4
        ioOCExpander.pinMode(11, INPUT); // Valve5
        ioOCExpander.pinMode(12, INPUT); // Valve6
        ioOCExpander.pinMode(13, INPUT); // HBridge1
        ioOCExpander.pinMode(14, INPUT); // HBridge2
        ioOCExpander.pinMode(15, INPUT); // HBridge3

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
