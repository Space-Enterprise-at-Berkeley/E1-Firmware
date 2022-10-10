#include <Common.h>
#include <Comms.h>

#include <Arduino.h>

#define LOX_RS485_SERIAL Serial1 // TODO: verify this
#define LOX_RS485_SW_PIN 36

char rs485Buffer[sizeof(Comms::Packet)];
int cnt = 0;
uint8_t state = 0;
uint8_t capID = 221;

void setup() {
    // hardware setup
    Serial.begin(115200);
    LOX_RS485_SERIAL.begin(115200);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif

    DEBUG("STARTING UP\n");
    DEBUG_FLUSH();

    pinMode(LOX_RS485_SW_PIN, OUTPUT);

    digitalWrite(LOX_RS485_SW_PIN, HIGH);
}

void loop() {
    // 0: ready to receive
    if (state == 0) { 
        DEBUG("Listening.....\n");
        DEBUG_FLUSH();
        while(LOX_RS485_SERIAL.available()) {
            rs485Buffer[cnt] = LOX_RS485_SERIAL.read();
            DEBUG(rs485Buffer[cnt]);
            DEBUG_FLUSH();
            if(cnt == 0 && rs485Buffer[cnt] != 221) {
                break;
            }
            if(rs485Buffer[cnt] == '\n') {
                Comms::Packet *packet = (Comms::Packet *)&rs485Buffer;
                if(Comms::verifyPacket(packet)) {
                    cnt = 0;
                    DEBUG("lox transmit command received! \n");
                    DEBUG_FLUSH();
                    Comms::emitPacket(packet);
                    state = 1; // switch back to transmit since full packet verified
                    break;
                }
            }
            cnt++;
        }
    }

    // 1: ready to transmit
    if (state == 1) { 
        DEBUG("in transmit mode\n");
        DEBUG_FLUSH();
        digitalWrite(LOX_RS485_SW_PIN, HIGH);
        Comms::Packet capCommand = {.id = 221};
        Comms::packetAddFloat(&capCommand, 1.234);
        Comms::emitPacket(&capCommand, LOX_RS485_SERIAL);
        LOX_RS485_SERIAL.flush();
        digitalWrite(LOX_RS485_SW_PIN, LOW);
        DEBUG("cap command sent\n");
        DEBUG_FLUSH();
        state = 0; // 0: ready to receive
    }

    if(cnt == 128) {
        DEBUG("\nRESETTING LOX BUFFER\n");
        cnt = 0;
        state = 0;
    }
    
}
