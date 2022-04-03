#include <Arduino.h>
#include <Wire.h>

uint8_t hb1_c1 = 0;
uint8_t hb1_c2 = 1;

uint8_t hb2_c1 = 15;
uint8_t hb2_c2 = 36;

uint8_t hb3_c1 = 21;
uint8_t hb3_c2 = 22;

uint8_t v1 = 7;
uint8_t v2 = 24;
uint8_t v3 = 2;
uint8_t v4 = 23;
uint8_t v5 = 9;
uint8_t v6 = 14;

int main() {
    pinMode(v1, OUTPUT);
    pinMode(v2, OUTPUT);
    pinMode(v3, OUTPUT);
    pinMode(v4, OUTPUT);
    pinMode(v5, OUTPUT);
    pinMode(v6, OUTPUT);

    pinMode(hb1_c1, OUTPUT);
    pinMode(hb1_c2, OUTPUT);
    pinMode(hb2_c1, OUTPUT);
    pinMode(hb2_c2, OUTPUT);
    pinMode(hb3_c1, OUTPUT);
    pinMode(hb3_c2, OUTPUT);
    
    while(1){
        digitalWrite(v2, HIGH);
        digitalWrite(v3, HIGH);
        digitalWrite(v4, HIGH);
        digitalWrite(v5, HIGH);
        digitalWrite(v6, HIGH);
        delay(1000);
        digitalWrite(v2, LOW);
        digitalWrite(v3, LOW);
        digitalWrite(v4, LOW);
        digitalWrite(v5, LOW);
        digitalWrite(v6, LOW);
        delay(1000);
    }
}