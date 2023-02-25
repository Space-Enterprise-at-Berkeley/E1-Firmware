#include <Common.h>
#include <Comms.h>

#include <Arduino.h>

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define LOX_SERIAL Serial5 // TODO: verify this
#define LOX_REN_PIN 22
#define FUEL_SERIAL Serial3 // TODO: verify this
#define FUEL_REN_PIN 18

char loxBuffer[sizeof(Comms::Packet)];
int loxCnt = 0;
char fuelBuffer[sizeof(Comms::Packet)];
int fuelCnt = 0;

void setup() {
    // hardware setup
    Serial.begin(115200);
    LOX_SERIAL.begin(921600);
    FUEL_SERIAL.begin(921600);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif

    Comms::initComms();

    DEBUG("STARTING UP\n");
    DEBUG_FLUSH();

    pinMode(LOX_REN_PIN, OUTPUT);
    pinMode(FUEL_REN_PIN, OUTPUT);

    digitalWriteFast(LOX_REN_PIN, LOW);
    digitalWriteFast(FUEL_REN_PIN, LOW);
}

void loop() {
        // if (LOX_SERIAL.available()) {
        //     Serial.println("hello world");
        // }

        // if (FUEL_SERIAL.available()) {
        //     Serial.println("hello fuel world");
        // }

        // while(LOX_SERIAL.available()) {
        //     loxBuffer[loxCnt] = LOX_SERIAL.read();
        //     loxCnt++;
        // }
        // Serial.write(loxBuffer, loxCnt);
        // loxCnt = 0;

        // while(FUEL_SERIAL.available()) {
        //     fuelBuffer[fuelCnt] = FUEL_SERIAL.read();
        //     fuelCnt++;
        // }
        // Serial.write(fuelBuffer, fuelCnt);
        // fuelCnt = 0;
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
                    Comms::emitPacket(packet);
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
