#include <Arduino.h>
#include <PCA9539.h>

PCA9539 ioOCExpander(0x74);
const uint8_t interruptPin = 20;

void OCInterrupt() {
    for (int i = 0; i <= 15; i++) {
        if (ioOCExpander.digitalRead(i)) {
            Serial.print("OC detected on pin: %i");
            Serial.println(i);
        }
    }
}

void printPinStatus() {
    //print expander pins
    for (int i = 0; i <= 15; i++) {
        uint8_t status = ioOCExpander.digitalRead(i);
        Serial.print("Pin ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(status);
    }

    //print interrupt pin
    Serial.print("Interrupt Pin: ");
    Serial.println(digitalRead(interruptPin));
}

int main() {
    Serial.begin(115200);

    ioOCExpander.pinMode(1, INPUT);
    ioOCExpander.pinMode(2, INPUT);
    ioOCExpander.pinMode(3, INPUT);
    ioOCExpander.pinMode(4, INPUT);
    ioOCExpander.pinMode(5, INPUT);
    ioOCExpander.pinMode(6, INPUT);
    ioOCExpander.pinMode(7, INPUT);
    ioOCExpander.pinMode(8, INPUT);
    ioOCExpander.pinMode(9, INPUT);
    ioOCExpander.pinMode(10, INPUT);
    ioOCExpander.pinMode(11, INPUT);
    ioOCExpander.pinMode(12, INPUT);
    ioOCExpander.pinMode(13, INPUT);
    ioOCExpander.pinMode(14, INPUT);
    ioOCExpander.pinMode(15, INPUT);

    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), OCInterrupt, LOW);

    while(1) {
        printPinStatus();
        delay(50);
    }
    
    return 0;
}
