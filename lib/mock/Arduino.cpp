#include <Arduino.h>

void pinMode(uint8_t pin, uint8_t mode) {

}

void digitalWrite(uint8_t pin, uint8_t mode) {

}

void analogWrite(uint8_t pin, uint8_t val) {

}

void digitalWriteFast(uint8_t pin, uint8_t val) {

}

void delay(uint32_t millis) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

uint32_t millis() {
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    return ms;
}

uint32_t micros() {
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    return us; 
}

void yield() {

}