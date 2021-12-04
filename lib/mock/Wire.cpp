#include <Wire.h>

TwoWire::TwoWire() {
    
}

void TwoWire::begin() {

}

void TwoWire::setClock(uint32_t baud) {
    TwoWire::baud = baud;
}

void TwoWire::beginTransmission(uint8_t address) {
  std::cout << "beginning transmission to: " << std::to_string((int) address) << "\n";
}

int TwoWire::write(uint8_t value) {
    return 0;
}

uint8_t TwoWire::endTransmission() {
    return NULL;
}

void TwoWire::send(uint8_t value) {

}

uint8_t TwoWire::requestFrom(uint8_t, uint8_t) {
    return 1;
}

uint8_t TwoWire::available() {
    return true;
}

uint8_t TwoWire::receive() {
    return 1;
}

uint8_t TwoWire::read() {
    return 1;
}

TwoWire Wire;
TwoWire Wire1;
