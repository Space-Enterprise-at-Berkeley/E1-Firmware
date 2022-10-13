#include "CapFill.h"

namespace CapFill {

    char rs485Buffer[sizeof(Comms::Packet)];
    int rs485BufferCnt = 0;

    uint8_t state = 0;

    // LOx Cap packet 221
    // Fuel Cap packet 222
    uint8_t capID = 221;

    uint8_t timeoutCount = 0;
    uint8_t cnt = 0;



    void initCapFill() {
        // Set pin to receive
        digitalWriteFast(&HAL::RS485SwitchPin, LOW); 
    }

    // Read from 1 cap fill sensor at a time
    uint32_t sampleCapFill() {

        // In Transmit Mode
        if (state == 0) { 
            digitalWriteFast(&HAL::RS485SwitchPin, HIGH);
            
            Comms::Packet capCommand = {.id = capID};
            // Comms::packetAddUint8(&capCommand, capID);
            Comms::emitPacket(&capCommand, RS485_SERIAL);
            
            DEBUG("cap command sent\n");
            DEBUG_FLUSH();

            RS485_SERIAL.flush();

            digitalWriteFast(&HAL::RS485SwitchPin, LOW); 
            
            DEBUG("cap set to receive\n");
            DEBUG_FLUSH();
            
            state = 1;
        }

        if (state == 1) { 
            DEBUG(".");
            DEBUG_FLUSH();
            timeoutCount++;
            if (timeoutCount > 1) { 
                // DEBUG("TIMED OUT\n");
                // DEBUG_FLUSH();
                state = 0;
                timeoutCount = 0;
                cnt = 0;
                return;
            }
            while(RS485_SERIAL.available()) {
                DEBUG("READING...\n");
                DEBUG_FLUSH();
                rs485Buffer[cnt] = RS485_SERIAL.read();
                // DEBUG((uint8_t)rs485Buffer[cnt]);
                // DEBUG_FLUSH();
                if(cnt == 0 && rs485Buffer[cnt] != capID) {
                    continue;
                } 
                // DEBUG("bruh\n");
                // DEBUG_FLUSH();
                if(rs485Buffer[cnt] == '\n') {
                    Comms::Packet *packet = (Comms::Packet *)&rs485Buffer;
                    if(Comms::verifyPacket(packet)) {
                        cnt = 0;
                        DEBUG("reading from: ");
                        DEBUG(packet->id);
                        DEBUG(" : ");
                        DEBUG(Comms::packetGetFloat(packet, 0));
                        DEBUG("\n");
                        DEBUG_FLUSH();
                        Comms::emitPacket(packet);
                        state = 0; // switch back to transmit since full packet verified
                        timeoutCount = 0;

                        // Switching cap ID to read other one next
                        (capID == 221) ? capID = 222: capID = 221;
 
                        // DEBUG("packet processed\n");
                        // DEBUG_FLUSH();
                        break;
                    }
                }
                cnt++;
            }
        }  

        if(cnt >= 42) {
            DEBUG("\nRESETTING BUFFER\n");
            cnt = 0;
            state = 0;
        }

        return 100 * 1000;  

    }

               
};
