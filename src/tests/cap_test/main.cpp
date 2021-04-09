#include <Arduino.h>

int main(int argc, char** argv) {
    uint32_t start = 0;
    uint32_t end = 0;
    uint8_t cycles = 0;
    Serial.begin(9600);
    pinMode(7, OUTPUT); // output
    pinMode(6, INPUT); // sense
    digitalWriteFast(7, LOW);

    sBit = PIN_TO_BITMASK(sendPin);					// get send pin's ports and bitmask
	sReg = PIN_TO_BASEREG(sendPin);					// get pointer to output register

	rBit = PIN_TO_BITMASK(receivePin);				// get receive pin's ports and bitmask
	rReg = PIN_TO_BASEREG(receivePin);

    while(true) {
        digitalWriteFast(7, HIGH);
        start = micros();
        while(true) {
            if(digitalReadFast(6)) {
                cycles += 1;
                end = micros();
                break;
            }
        }
        Serial.println(end-start);
        Serial.println(cycles);
        cycles = 0;
        digitalWriteFast(7, LOW);
        delay(1000);
    }
}
