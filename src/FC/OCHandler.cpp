#include "OCHandler.h"

namespace OCHandler {

    PCA9539 ioOCExpander(0x24);

    uint32_t ocUpdatePeriod = 100 * 1000;

    bool ocDetected = false;

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
        DEBUG("init all io expand pins\n");
        // pinMode(interruptPin, INPUT_PULLUP);
        // attachInterrupt(digitalPinToInterrupt(interruptPin), OCInterrupt, LOW);
        DEBUG('e\n');
    }

    Comms::Packet ocPacket = {.id = 40};
    
    void OCInterrupt() {
        ocDetected = true;
    }

    uint32_t handleOC() { 

        if (!ocDetected) {
            return ocUpdatePeriod;
        } 

        ocPacket.len = 0;
        Comms::packetAddUint16(&ocPacket, ioOCExpander.digitalReadAll());
        Comms::emitPacket(&ocPacket);
        return ocUpdatePeriod;
    }
}
