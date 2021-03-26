
#include <Arduino.h>
#include <SPI.h>
#include <ADS8167.h>
#include <Wire.h>

//ADC 1
uint8_t cs_pin1 = 37;
uint8_t rdy_pin1 = 26;
uint8_t alrt_pin1 = 9;

//ADC 2
uint8_t cs_pin2 = 36;
uint8_t rdy_pin2 = 27;
uint8_t alrt_pin2 = 10;

ADS8167 adc1(&SPI, cs_pin1, rdy_pin1, alrt_pin1);
ADS8167 adc2(&SPI, cs_pin2, rdy_pin2, alrt_pin2);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n");

  adc1.init();
  adc1.setManualMode();
  adc1.setAllInputsSeparate();
  pinMode(rdy_pin1, INPUT_PULLUP);

  adc2.init();
  adc2.setManualMode();
  adc2.setAllInputsSeparate();
  pinMode(rdy_pin2, INPUT_PULLUP);
}

void loop() {
  Serial.println("\n");
  for (int i = 0; i < 8; i ++) {
    // Serial.print("A");
    // Serial.print(i);
    Serial.print(adc1.readData(i));
    Serial.print(", ");
  }
  Serial.println();
  for (int i = 0; i < 8; i ++) {
    // Serial.print("A");
    // Serial.print(i);
    Serial.print(adc2.readData(i));
    Serial.print(", ");
  }
  Serial.println();
  delay(1000);
}
