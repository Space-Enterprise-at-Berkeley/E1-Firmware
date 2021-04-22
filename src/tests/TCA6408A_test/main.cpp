
#include <Arduino.h>
#include <SPI.h>
#include <GpioExpander.h>
#include <Wire.h>

//TCA6408A 1
uint8_t _addr = TCA6408A_ADDR1;
uint8_t alrt_pin = -1;

//TCA6408A 2
// uint8_t _addr = TCA6408A_ADDR2;
// uint8_t alrt_pin = 33;

GpioExpander tca(_addr, alrt_pin, &Wire);
bool states[8] = {0,0,0,0,0,0,0,0};
void setup() {
  Serial.begin(9600);
  delay(1000);
  Wire.begin();
  tca.init();
}

void loop() {
  for (int i = 0; i < 8; i ++) {
    states[i] = states[i] == 0 ? 1 : 0;
    if (states[i]) {
      tca.turnOn(i);
    } else {
      tca.turnOff(i);
    }
    Serial.print("O");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(states[i]);
    delay(500);
  }
}
