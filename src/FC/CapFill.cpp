#include "CapFill.h"

namespace CapFill {

    char rs485Buffer[sizeof(Comms::Packet)];
    uint8_t cnt = 0;

    void initCapFill() {
        // Set pin to receive
        digitalWriteFast(HAL::RS485SwitchPin, LOW); 
    }

    // Read from 1 cap fill sensor at a time
    uint32_t sampleCapFill() {
        cnt = 0;
        while(RS485_SERIAL.available()) {
            rs485Buffer[cnt] = RS485_SERIAL.read();
            if(rs485Buffer[cnt] == '\n') {
                Comms::Packet *packet = (Comms::Packet *)&rs485Buffer;
                if(Comms::verifyPacket(packet)) {
                    DEBUG("Found cap fill packet with ID ");
                    DEBUG(packet->id);
                    DEBUG('\n');
                    Comms::emitPacket(packet);
                    Comms::emitPacket(packet, &RADIO_SERIAL, "\r\n\n", 3);
                    break;
                }
            }
            cnt++;
        }
        return 40 * 1000;
    }
};
