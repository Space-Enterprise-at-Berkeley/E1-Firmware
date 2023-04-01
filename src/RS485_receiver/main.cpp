#include <Common.h>
#include <Comms.h>

#include <Arduino.h>

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define LOX_SERIAL Serial4 // TODO: verify this
#define LOX_REN_PIN 19
#define FUEL_SERIAL Serial3 // TODO: verify this
#define FUEL_REN_PIN 18

char loxBuffer[sizeof(Comms::Packet)];
int loxCnt = 0;
char fuelBuffer[sizeof(Comms::Packet)];
int fuelCnt = 0;

void setup() {
    // hardware setup
    Serial.begin(115200);
    LOX_SERIAL.begin(115200);
    FUEL_SERIAL.begin(115200);

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
            if(fuelCnt == 0 && ((uint8_t)fuelBuffer[0] != 21 && (uint8_t) fuelBuffer[0] != 22)) {
                Serial.print((uint8_t)fuelBuffer[0]);
                Serial.print(" ");
                continue;
            }
            if(fuelBuffer[fuelCnt] == '\n') {
                Serial.println("got newline");
                Comms::Packet *packet = (Comms::Packet *)&fuelBuffer;
                if(Comms::verifyPacket(packet)) {
                    fuelCnt = 0;
                    Serial.print("fuel: ");
                    Serial.print(packet->id);
                    Serial.print(" : ");
                    Serial.print(Comms::packetGetFloat(packet, 0));
                    Serial.print("\n");
                    Comms::emitPacket(packet);
                    break;
                }
            }
            fuelCnt++;
        }
        if(fuelCnt == 256) {
            Serial.println("RESETTING FUEL BUFFER\n");
            fuelCnt = 0;
        }


        while(LOX_SERIAL.available() && loxCnt < 256) {
            loxBuffer[loxCnt] = LOX_SERIAL.read();
            if(loxCnt == 0 && ((uint8_t)loxBuffer[0] != 21 && (uint8_t) loxBuffer[0] != 22)) {
                Serial.print((uint8_t)loxBuffer[0]);
                Serial.print(" ");
                continue;
            }
            if(loxBuffer[loxCnt] == '\n') {
                Serial.println("got newline");
                Comms::Packet *packet = (Comms::Packet *)&loxBuffer;
                if(Comms::verifyPacket(packet)) {
                    loxCnt = 0;
                    Serial.print("lox: ");
                    Serial.print(packet->id);
                    Serial.print(" : ");
                    Serial.print(Comms::packetGetFloat(packet, 0));
                    Serial.print("\n");
                    Comms::emitPacket(packet);
                    break;
                }
            }
            loxCnt++;
        }
        if(loxCnt == 256) {
            Serial.println("RESETTING LOX BUFFER\n");
            loxCnt = 0;
        }

    }
