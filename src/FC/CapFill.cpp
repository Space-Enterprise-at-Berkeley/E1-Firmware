#include "CapFill.h"

namespace CapFill {

    char rs485Buffer[sizeof(Comms::Packet)];
    int rs485BufferCnt = 0;

    bool loxCapLastRead = false;
    bool hasTransmitted = false;

    // LOx Cap packet 221
    // Fuel Cap packet 222

    void initCapFill() {
        // Set pin to receive
        // digitalWriteFast(&HAL::RS485SwitchPin, LOW); 
    }

    // Read from 1 cap fill sensor at a time
    uint32_t sampleCapFill() {
        // Set pin to transmit
        DEBUG("sample cap running\n");
        DEBUG_FLUSH();

        DEBUG("Transmitted to cap: ");
        DEBUG(hasTransmitted);
        DEBUG("\n");
        DEBUG_FLUSH();

        if (!hasTransmitted) { 
            digitalWriteFast(&HAL::RS485SwitchPin, HIGH);
            if (loxCapLastRead) { 
                
            }
            Comms::Packet capCommand = {.id = 221};
            Comms::emitPacket(&capCommand, RS485_SERIAL);
            DEBUG("cap command sent\n");
            DEBUG_FLUSH();
            RS485_SERIAL.flush();
            digitalWriteFast(&HAL::RS485SwitchPin, LOW); 
            DEBUG("cap set to receive\n");
            DEBUG_FLUSH();
            hasTransmitted = true;
        }

        while (RS485_SERIAL.available()) { 
            DEBUG("Just read lox cap? ");
            DEBUG(loxCapLastRead);
            DEBUG("\n");
            DEBUG_FLUSH();
            if (loxCapLastRead) {
                // Sample fuel cap
                DEBUG(".... Reading fuel cap ....\n");
                DEBUG_FLUSH();
                sampleCapBoard(222);
                loxCapLastRead = false;
            } else {
                // Sample LOx cap
                DEBUG(".... Reading lox cap ....\n");
                DEBUG_FLUSH();
                sampleCapBoard(221);
                loxCapLastRead = true;
            }
            hasTransmitted = false;
        }

        // digitalWriteFast(&HAL::RS485SwitchPin, HIGH);
        // Comms::Packet capCommand = {.id = 221};
        // Comms::emitPacket(&capCommand, RS485_SERIAL);
        // DEBUG("Cap command sent\n");
        // DEBUG_FLUSH();
        // RS485_SERIAL.flush();

        return 100 * 1000; 
    
        // while (RS485_SERIAL.available()) {
            // if (loxCapLastRead) {
            //     sampleFuelCap();
            //     loxCapLastRead = false;
            // } else {
            //     sampleLOxCap();
            //     loxCapLastRead = true;
            // }
            // sampleLOxCap();
            // while(rs485BufferCnt < 256) {
            //     rs485Buffer[rs485BufferCnt] = RS485_SERIAL.read();
            //     if(rs485BufferCnt == 0 && rs485Buffer[rs485BufferCnt] != 221) {
            //         break;
            //     }
            //     if(rs485Buffer[rs485BufferCnt] == '\n') {
            //         Comms::Packet *packet = (Comms::Packet *)&rs485Buffer;
            //         DEBUG("Cap packet received\n");
            //         DEBUG_FLUSH();
            //         if(Comms::verifyPacket(packet)) {
            //             DEBUG("Cap packet confirmed\n");
            //             DEBUG_FLUSH();
            //             rs485BufferCnt = 0;
            //             DEBUG("reading: ");
            //             DEBUG(packet->id);
            //             DEBUG(" : ");
            //             DEBUG(Comms::packetGetFloat(packet, 0));
            //             DEBUG("\n");
            //             DEBUG_FLUSH();
            //             Comms::emitPacket(packet);
            //             hasTransmitted = false;
            //             break;
            //         }
            //     }
            //     rs485BufferCnt++;
            // }
            // if(rs485BufferCnt == 256) {
            //     DEBUG("\nRESETTING BUFFER\n");
            //     rs485BufferCnt = 0;
            // }
        //}
    }

    void sampleCapBoard(uint8_t capID) {
        Comms::Packet capCommand = {.id = capID};
        Comms::emitPacket(&capCommand, RS485_SERIAL);

        while (RS485_SERIAL.available()) {
            while(rs485BufferCnt < 256) {
                rs485Buffer[rs485BufferCnt] = RS485_SERIAL.read();
                if(rs485BufferCnt == 0 && rs485Buffer[rs485BufferCnt] != capID) {
                    break;
                }
                if(rs485Buffer[rs485BufferCnt] == '\n') {
                    Comms::Packet *packet = (Comms::Packet *)&rs485Buffer;
                    if(Comms::verifyPacket(packet)) {
                        rs485BufferCnt = 0;
                        DEBUG("reading: ");
                        DEBUG(packet->id);
                        DEBUG(" : ");
                        DEBUG(Comms::packetGetFloat(packet, 0));
                        DEBUG("\n");
                        DEBUG_FLUSH();
                        Comms::emitPacket(packet);
                        break;
                    }
                }
                rs485BufferCnt++;
            }
            if(rs485BufferCnt == 256) {
                DEBUG("\nRESETTING BUFFER\n");
                rs485BufferCnt = 0;
            }
        }
    }
};
