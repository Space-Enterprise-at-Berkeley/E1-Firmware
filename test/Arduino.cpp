#include <Arduino.h>

auto _start_time = std::chrono::high_resolution_clock::now();

void pinMode(uint8_t pin, uint8_t mode) {
  std::cout << "set pin " << pin << " to mode " << mode << std::endl;
}

void digitalWrite(uint8_t pin, uint8_t val) {
  std::cout << "set pin " << pin << " to state " << val << std::endl;
}

void digitalWriteFast(uint8_t pin, uint8_t val) {
  std::cout << "set pin " << pin << " to state " << val << " (fast)" << std::endl;
}

void delay(uint32_t millis) {
  std::cout << "delaying for " << millis << " milliseconds" << std::endl;
  usleep(millis * 1000);
}

// uint32_t millis() {
//   std::chrono::high_resolution_clock::now();
// }

void yield() {}
