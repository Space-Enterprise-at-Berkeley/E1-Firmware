#include <Arduino.h>

const uint8_t aPin0 = 40;
const uint8_t aPin1 = 16;

const uint8_t sel0 = 3;
const uint8_t sel1 = 4;
const uint8_t sel2 = 5;
const uint8_t sel3 = 6;

const uint8_t wr = 41;

int main() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("---BEGIN---");

    analogReadRes(12);

    pinMode(wr, OUTPUT);
    
    pinMode(aPin0, INPUT);
    pinMode(aPin1, INPUT);

    pinMode(sel0, OUTPUT);
    pinMode(sel1, OUTPUT);
    pinMode(sel2, OUTPUT);
    pinMode(sel3, OUTPUT);

    digitalWriteFast(wr, 0);

    delay(1);

    digitalWriteFast(sel0, 1);
    digitalWriteFast(sel1, 1);
    digitalWriteFast(sel2, 1);
    digitalWriteFast(sel3, 0);

    delay(1);

    digitalWriteFast(wr, 1);

    while(1) {
        // for(uint8_t i = 0; i < 16; i++) {
        //     Serial.print(i & 0x1);
        //     Serial.print((i >> 1) & 0x1);
        //     Serial.print((i >> 2) & 0x1);
        //     Serial.print((i >> 3) & 0x1);
        //     Serial.print(" ");
        //     digitalWriteFast(sel0, i & 0x1);
        //     digitalWriteFast(sel1, (i >> 1) & 0x1);
        //     digitalWriteFast(sel2, (i >> 2) & 0x1);
        //     digitalWriteFast(sel3, (i >> 3) & 0x1);
        //     delay(1);
        //     float a0 = (float)analogRead(aPin0) * 3.3 / 4096.0;
        //     float a1 = (float)analogRead(aPin1) * 3.3 / 4096.0;
        //     Serial.print(a0);
        //     Serial.print(":");
        //     Serial.print(a1);
        //     Serial.print(" ");
        //     Serial.println();
        // }

        digitalWriteFast(sel0, 1);
        digitalWriteFast(sel1, 1);
        digitalWriteFast(sel2, 1);
        digitalWriteFast(sel3, 0);

        float a0 = ((float)analogRead(aPin0)) * 3.3 / 4096.0;
        float a1 = ((float)analogRead(aPin1)) * 3.3 / 4096.0;
        Serial.print(a0);
        Serial.print(":");
        Serial.print(a1);
        Serial.print(" ");

        Serial.println();
        delay(500);
    }

    return 0;
}
