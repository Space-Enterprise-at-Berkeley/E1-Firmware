#include <Common.h>
#include <Comms.h>

#include <Arduino.h>

Task taskTable[] = {
    
};

#define TASK_COUNT (sizeof(taskTable) / sizeof(struct Task))

#define RS485_SERIAL Serial8 // TODO: verify this
#define RS485_SW_PIN 27 
// #define FUEL_SERIAL Serial4 // TODO: verify this
// #define FUEL_REN_PIN 18

#define RS485_TX_PIN 35

char rs485Buffer[sizeof(Comms::Packet)];
int cnt = 0;
// char fuelBuffer[sizeof(Comms::Packet)];
// int fuelCnt = 0;
uint8_t state = 0;
uint8_t capID = 221;

int main() {
    // hardware setup
    Serial.begin(115200);
    RS485_SERIAL.begin(115200);
    // #ifdef DEBUG_MODE
    // while(!Serial) {} // wait for user to open serial port (debugging only)
    // #endif

    // Comms::initComms();

    DEBUG("STARTING UP\n");
    DEBUG_FLUSH();

    pinMode(RS485_SW_PIN, OUTPUT);
    // pinMode(FUEL_REN_PIN, OUTPUT);

    digitalWriteFast(RS485_SW_PIN, HIGH)                                                                                                                                                                                                                                                                                                                                                                      ;
    // digitalWriteFast(FUEL_REN_PIN, LOW);

    // pinMode(RS485_TX_PIN, OUTPUT);

    // digitalWriteFast(RS485_TX_PIN, HIGH);


    while(1) {

        // digitalWriteFast(RS485_TX_PIN, HIGH);
        // delay(2000);
        // digitalWriteFast(RS485_TX_PIN, LOW);
        // delay(2000);

        // 0: ready to transmit
        if (state == 0) { 
            DEBUG("in transmit mode\n");
            DEBUG_FLUSH();
            digitalWriteFast(RS485_SW_PIN, HIGH);
            // Comms::Packet capCommand = {.id = 221}; 
            // Comms::packetAddFloat(&capCommand, 221);
            // Comms::emitPacket(&capCommand, RS485_SERIAL);

            RS485_SERIAL.write("e");
            DEBUG("cap command sent\n");
            DEBUG_FLUSH();
            RS485_SERIAL.flush();
            // digitalWriteFast(RS485_SW_PIN, LOW);
            // digitalWriteFast(RS485_SW_PIN, LOW); 
            // DEBUG("cap set to receive\n");
            // DEBUG_FLUSH();
            // state = 1; // 1: ready to receive
        }

        if (state == 1) { 
            DEBUG("Listening.....\n");
            DEBUG_FLUSH();
            while(RS485_SERIAL.available()) {
                DEBUG("READING...\n");
                DEBUG_FLUSH();
                rs485Buffer[cnt] = RS485_SERIAL.read();
                DEBUG(rs485Buffer[cnt]);
                DEBUG("\n");
                DEBUG_FLUSH();
                if(cnt == 0 && rs485Buffer[cnt] != 221) {
                    break;
                }
                if(rs485Buffer[cnt] == '\n') {
                    Comms::Packet *packet = (Comms::Packet *)&rs485Buffer;
                    if(Comms::verifyPacket(packet)) {
                        cnt = 0;
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
                cnt++;
            }
        }  

        if(cnt == 256) {
            DEBUG("\nRESETTING LOX BUFFER\n");
            cnt = 0;
            state = 0;
        }
    }

    return 0;
}
