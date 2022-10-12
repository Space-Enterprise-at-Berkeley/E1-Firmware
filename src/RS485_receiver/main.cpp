#include <Common.h>
#include <Comms.h>

#include <Arduino.h>

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define LOX_SERIAL Serial5 // TODO: verify this
#define LOX_REN_PIN 22
// #define FUEL_SERIAL Serial4 // TODO: verify this
// #define FUEL_REN_PIN 18

char loxBuffer[sizeof(Comms::Packet)];
int loxCnt = 0;
// char fuelBuffer[sizeof(Comms::Packet)];
// int fuelCnt = 0;

int main() {
    // hardware setup
    Serial.begin(115200);
    LOX_SERIAL.begin(115200);
    // FUEL_SERIAL.begin(115200);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif

    // Comms::initComms();

    DEBUG("STARTING UP\n");
    DEBUG_FLUSH();

    pinMode(LOX_REN_PIN, OUTPUT);
    // pinMode(FUEL_REN_PIN, OUTPUT);

    digitalWriteFast(LOX_REN_PIN, LOW);
    // digitalWriteFast(FUEL_REN_PIN, LOW);


    while(1) {

        while(LOX_SERIAL.available()) {
            loxBuffer[loxCnt] = LOX_SERIAL.read();
            // DEBUG(loxBuffer[loxCnt]);
            // DEBUG_FLUSH();
            if(loxCnt == 0 && loxBuffer[loxCnt] != 221) {
                break;
            }
            if(loxBuffer[loxCnt] == '\n') {
                Comms::Packet *packet = (Comms::Packet *)&loxBuffer;
                if(Comms::verifyPacket(packet)) {
                    DEBUG(packet->id);
                    DEBUG("\n");
                    DEBUG_FLUSH();
                    loxCnt = 0;
                    if(packet->id == 221) {
                        DEBUG("got\n");
                        DEBUG_FLUSH();
                        
                        digitalWriteFast(LOX_REN_PIN, HIGH);
                        Comms::Packet tmp = {.id = 221};
                        Comms::packetAddFloat(&tmp, 1.234);
                        packet->data[0] = 0xaa;
                        // LOX_SERIAL.write(packet->id);
                        // LOX_SERIAL.write(packet->len);
                        // LOX_SERIAL.write(packet->timestamp, 4);
                        // LOX_SERIAL.write(packet->checksum, 2);
                        // LOX_SERIAL.write(packet->data, packet->len);
                        // LOX_SERIAL.write('\n');
                        // LOX_SERIAL.flush();
                        Comms::emitPacket(packet, &LOX_SERIAL);
                        LOX_SERIAL.flush();

                        digitalWriteFast(LOX_REN_PIN, LOW);
                    }
                    break;
                }
            }
            loxCnt++;
        }
        if(loxCnt == 128) {
            DEBUG("\nRESETTING LOX BUFFER\n");
            loxCnt = 0;
        }

        // while(FUEL_SERIAL.available() && fuelCnt < 256) {
        //     fuelBuffer[fuelCnt] = FUEL_SERIAL.read();
        //     if(fuelBuffer[fuelCnt] == '\n') {
        //         Comms::Packet *packet = (Comms::Packet *)&fuelBuffer;
        //         if(Comms::verifyPacket(packet)) {
        //             fuelCnt = 0;
        //             DEBUG("fuel: ");
        //             DEBUG(packet->id);
        //             DEBUG(" : ");
        //             DEBUG(Comms::packetGetFloat(packet, 0));
        //             DEBUG("\n");
        //             DEBUG_FLUSH();
        //             Comms::emitPacket(packet);
        //             break;
        //         }
        //     }
        //     fuelCnt++;
        // }
        // if(fuelCnt == 256) {
        //     DEBUG("RESETTING FUEL BUFFER\n");
        //     fuelCnt = 0;
        // }

    }


    // while(1) {
    //     uint32_t ticks = micros(); // current time in microseconds
    //     for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
    //     if (taskTable[i].enabled && ticks >= taskTable[i].nexttime)
    //         // DEBUG(i);
    //         // DEBUG("\n");
    //         taskTable[i].nexttime = ticks + taskTable[i].taskCall();
    //     }
    //     Comms::processWaitingPackets();
    // }
    return 0;
}
