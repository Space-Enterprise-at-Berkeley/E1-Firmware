#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

uint8_t _cs_pin = 36;
uint8_t _rdy_pin = 27;

uint8_t buffer[6];


void setup(){
  SPI.begin();
  Serial.begin(9600);
  delay(1000);
  Serial.println("begin");

  pinMode(_cs_pin, OUTPUT);
  pinMode(_rdy_pin, INPUT);
  digitalWrite(_cs_pin, HIGH);

  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0)); // chip uses mode 0 by default
  digitalWrite(_cs_pin, LOW);

  buffer[0] = 0b00001000;
  buffer[1] = 0x00;
  buffer[2] = 0b10101010;

  SPI.transfer(buffer, 3);

  digitalWrite(_cs_pin, HIGH);
  SPI.endTransaction();
}

void loop(){
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0)); // chip uses mode 0 by default
  digitalWrite(_cs_pin, LOW);

  buffer[0] = 0x08;
  buffer[1] = 0x1D;
  buffer[2] = 0x00;
  buffer[3] = 0x00;

  SPI.transfer(buffer, 4);

  digitalWrite(_cs_pin, HIGH);
  SPI.endTransaction();

  Serial.println(buffer[0], BIN);
  Serial.println(buffer[1], BIN);
  Serial.println(buffer[2], BIN);
  Serial.println(buffer[3], BIN);
  uint16_t read = ((buffer[1] << 12) & 0xF000) | buffer[2] | ((buffer[3] >> 4) & 0x0F);
  Serial.println(read);
  Serial.flush();
  delay(1000);
}
