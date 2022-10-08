#include <Common.h>
#include <Comms.h>

#include <Arduino.h>

Task taskTable[] = {
    
};

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define CAP_SERIAL Serial3 // TODO: verify this
#define LOX_REN_PIN 19
// #define FUEL_SERIAL Serial4 // TODO: verify this
// #define FUEL_REN_PIN 18

char loxBuffer[sizeof(Comms::Packet)];
int loxCnt = 0;
// char fuelBuffer[sizeof(Comms::Packet)];
// int fuelCnt = 0;
uint8_t state = 0;

int main() {
    // hardware setup
    Serial.begin(115200);
    CAP_SERIAL.begin(115200);
    // FUEL_SERIAL.begin(115200);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif

    Comms::initComms();

    DEBUG("STARTING UP\n");
    DEBUG_FLUSH();

    pinMode(LOX_REN_PIN, OUTPUT);
    // pinMode(FUEL_REN_PIN, OUTPUT);

    digitalWriteFast(LOX_REN_PIN, LOW);
    // digitalWriteFast(FUEL_REN_PIN, LOW);


    while(1) {

        // 0: ready to transmit
        if (state == 0) { 
            digitalWriteFast(LOX_REN_PIN, HIGH);
            Comms::Packet capCommand = {.id = 221};
            Comms::emitPacket(&capCommand, CAP_SERIAL);
            DEBUG("cap command sent\n");
            DEBUG_FLUSH();
            RS485_SERIAL.flush();
            digitalWriteFast(LOX_REN_PIN, LOW); 
            DEBUG("cap set to receive\n");
            DEBUG_FLUSH();
            state == 1; // 1: ready to receive
        }

        if (state == 1) { 
            while(CAP_SERIAL.available() && loxCnt < 256) {
                loxBuffer[loxCnt] = CAP_SERIAL.read();
                DEBUG(loxBuffer[loxCnt]);
                DEBUG("\n");
                DEBUG_FLUSH();
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
                        Comms::emitPacket(packet);
                        state = 0; // switch back to transmit since full packet verified
                        break;
                    }
                }
                loxCnt++;
            }
        }  

        if(loxCnt == 256) {
            DEBUG("\nRESETTING LOX BUFFER\n");
            loxCnt = 0;
            state = 0;
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

    return 0;
}
