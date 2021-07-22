#include <Arduino.h>

auto _start_time = chrono::high_resolution_clock::now();

void pinMode(uint8_t pin, uint8_t mode) {
  cout << "set pin " << to_string(pin) << " to mode " << to_string(mode) << endl;
}

void digitalWrite(uint8_t pin, uint8_t val) {
  cout << "set pin " << to_string(pin) << " to state " << to_string(val) << endl;
}

void digitalWriteFast(uint8_t pin, uint8_t val) {
  cout << "set pin " << to_string(pin) << " to state " << to_string(val) << " (fast)" << endl;
}

void delay(uint32_t millis) {
  cout << "delaying for " << to_string(millis) << " milliseconds" << endl;
  this_thread::sleep_for(chrono::milliseconds(millis));
}

uint32_t millis() {
  auto end = chrono::high_resolution_clock::now();
  return chrono::duration_cast<chrono::microseconds>(end - _start_time).count();
}

void yield() {}
