#include <Common.h>
#include <Comms.h>

#include <Arduino.h>

Task taskTable[] = {
    
};

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define LOX_SERIAL Serial3 // TODO: verify this
#define LOX_REN_PIN 19
#define FUEL_SERIAL Serial4 // TODO: verify this
#define FUEL_REN_PIN 18

char loxBuffer[sizeof(Comms::Packet)];
int loxCnt = 0;
char fuelBuffer[sizeof(Comms::Packet)];
int fuelCnt = 0;

int main() {
    // hardware setup
    Serial.begin(115200);
    LOX_SERIAL.begin(115200);
    FUEL_SERIAL.begin(115200);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif

    // Comms::initComms();

    DEBUG("STARTING UP\n");
    DEBUG_FLUSH();

    pinMode(LOX_REN_PIN, OUTPUT);
    pinMode(FUEL_REN_PIN, OUTPUT);

    digitalWriteFast(LOX_REN_PIN, LOW);
    digitalWriteFast(FUEL_REN_PIN, LOW);


    while(1) {

        while(LOX_SERIAL.available() && loxCnt < 256) {
            loxBuffer[loxCnt] = LOX_SERIAL.read();
            if(loxCnt == 0 && loxBuffer[loxCnt] != 221) {
                break;
            }
            if(loxBuffer[loxCnt] == '\n') {
                Comms::Packet *packet = (Comms::Packet *)&loxBuffer;
                if(Comms::verifyPacket(packet)) {
                    loxCnt = 0;
                    DEBUG("lox: ");
                    DEBUG(packet->id);
                    DEBUG(" : ");
                    DEBUG(Comms::packetGetFloat(packet, 0));
                    DEBUG("\n");
                    DEBUG_FLUSH();
                    // Comms::emitPacket(packet);
                    break;
                }
            }
            loxCnt++;
        }
        if(loxCnt == 256) {
            DEBUG("\nRESETTING LOX BUFFER\n");
            loxCnt = 0;
        }

        while(FUEL_SERIAL.available() && fuelCnt < 256) {
            fuelBuffer[fuelCnt] = FUEL_SERIAL.read();
            if(fuelBuffer[fuelCnt] == '\n') {
                Comms::Packet *packet = (Comms::Packet *)&fuelBuffer;
                if(Comms::verifyPacket(packet)) {
                    fuelCnt = 0;
                    DEBUG("fuel: ");
                    DEBUG(packet->id);
                    DEBUG(" : ");
                    DEBUG(Comms::packetGetFloat(packet, 0));
                    DEBUG("\n");
                    DEBUG_FLUSH();
                    // Comms::emitPacket(packet);
                    break;
                }
            }
            fuelCnt++;
        }
        if(fuelCnt == 256) {
            DEBUG("RESETTING FUEL BUFFER\n");
            fuelCnt = 0;
        }

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
