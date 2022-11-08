#include <Arduino.h>

#define RS485_SERIAL Serial1
#define RS485_SW_PIN 22 

void setup() {
    Serial.begin(115200);
    RS485_SERIAL.begin(921600);
    pinMode(RS485_SW_PIN, OUTPUT);
    digitalWrite(RS485_SW_PIN, HIGH);
}

void loop() {
    for(int i = 0; i < 276; i++) {
        RS485_SERIAL.write('\n');
    }
    RS485_SERIAL.flush();
    Serial.println("yeet");
    delay(1000);
}
