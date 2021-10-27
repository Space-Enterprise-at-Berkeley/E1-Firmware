#include <Wire.h>

TwoWire::TwoWire() {
    
}

void TwoWire::begin() {

}

void TwoWire::setClock(uint32_t baud) {

}

void TwoWire::beginTransmission(uint8_t address) {

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
    return NULL;
}

uint8_t TwoWire::available() {
    return NULL;
}

uint8_t TwoWire::receive() {
    return NULL;
}

uint8_t TwoWire::read() {
    return NULL;
}

TwoWire Wire;
TwoWire Wire1;
