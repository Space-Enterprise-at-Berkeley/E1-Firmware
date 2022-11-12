#include <Arduino.h>

const uint8_t aPin0 = 40;
const uint8_t aPin1 = 16;

const uint8_t sel0 = 3;
const uint8_t sel1 = 4;
const uint8_t sel2 = 5;
const uint8_t sel3 = 6;

const uint8_t wr = 41;

uint8_t v1 = 24; // 7;
uint8_t count = 0;
uint8_t hb2_c1 = 15;
uint8_t hb2_c2 = 36;

int main() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("---BEGIN---");

    analogReadRes(12);

    pinMode(v1, OUTPUT);
    pinMode(hb2_c1, OUTPUT);
    pinMode(hb2_c2, OUTPUT);

    pinMode(wr, OUTPUT);
    
    pinMode(aPin0, INPUT);
    pinMode(aPin1, INPUT);

    pinMode(sel0, OUTPUT);
    pinMode(sel1, OUTPUT);
    pinMode(sel2, OUTPUT);
    pinMode(sel3, OUTPUT);

    digitalWriteFast(wr, 0);

    delay(1);

    while(1) {
        Serial.println("-----Taking sample-----");
        if (count == 50) {
           // digitalWrite(v1, HIGH)  ;
           digitalWrite(hb2_c1, HIGH);
        }
        if (count == 100) { 
            count = 0;
           // digitalWrite(v1, LOW);
           digitalWrite(hb2_c1, LOW);
        }
        for(uint8_t i = 0; i < 16; i++) {
            Serial.print(i & 0x1);
            Serial.print((i >> 1) & 0x1);
            Serial.print((i >> 2) & 0x1);
            Serial.print((i >> 3) & 0x1);
            Serial.print(" (");
            Serial.print(i);
            Serial.print(") ");
            digitalWriteFast(sel0, i & 0x1);
            digitalWriteFast(sel1, (i >> 1) & 0x1);
            digitalWriteFast(sel2, (i >> 2) & 0x1);
            digitalWriteFast(sel3, (i >> 3) & 0x1);
            delay(1);
            float a0 = (float)analogRead(aPin0) * ((1.0 / 20.0) / 0.02) * 3.3 / 4096.0;
            float a1 = (float)analogRead(aPin1) * (11.0) / 4096.0;
    //            const float valveMuxCurrentScalingFactor = ((1.0 / 20.0) / 0.02) * 3.3 / 4096.0; // current
    // const float valveMuxContinuityScalingFactor = (11.0) / 4096.0; // "voltage" reading, TODO will this value need to change
            Serial.print(a0, 5);
            Serial.print(":");
            Serial.println(a1, 5);
            // Serial.print(" , Shunt V: ");
            // Serial.print(a1 / 20, 5);
            // Serial.print(" , Shunt Current: ");
            // Serial.print((a0 / 20) / 0.02, 5);
            // Serial.println();
        }
        delay(100);
        count++; 
    }

    return 0;
}
