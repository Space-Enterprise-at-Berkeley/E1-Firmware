/*
 Basic Load Cell Operation in Kg
 Arduino pins
 5V -> VCC
 3.3V -> VDD
 3 -> DAT
 2 -> CLK
 GND -> GND

*/

#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711

#define lc1cal_factor -3950.0
#define lc2cal_factor 4000

#define lc1_DOUT_PIN  A0
#define lc1_SCK_PIN  A1

#define lc2_DOUT_PIN  A2
#define lc2_SCK_PIN  A3

HX711 lc1;
HX711 lc2;

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("HX711 scale");
  Serial.flush();
  

  lc1.begin(lc1_DOUT_PIN, lc1_SCK_PIN);
  lc1.set_scale(lc1cal_factor);
  lc1.tare();

  lc2.begin(lc2_DOUT_PIN, lc2_SCK_PIN);
  lc2.set_scale(lc2cal_factor);
  lc2.tare();

  Serial.flush();
}

void loop() {
  Serial.println("Reading: ");
  Serial.print("LC1: ");
  Serial.print(lc1.get_units() * 0.453592, 1);
  Serial.println(" kg");
  Serial.print("LC2: ");
  Serial.print(lc2.get_units() * 0.453592, 1);
  Serial.println(" kg");
  Serial.println();
}
