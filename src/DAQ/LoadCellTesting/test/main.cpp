#include <Arduino.h>
#include <Wire.h> 
#include <HX711.h>

HX711 lcAmp0;
HX711 lcAmp1;


float lcSample(HX711 *amp) {
        return amp->get_units(); // pounds
    }

int main() {
    
    lcAmp0.init(9, 10, 3889); // data, clk, gain 
    lcAmp1.init(39, 38, 3941); // data, clk, gain 
    lcAmp0.tare();
    lcAmp1.tare();

  while(1) {
    // Serial.println("LC0");
    // Serial.println(lcSample(&lcAmp0));
    Serial.println("LC1");
    Serial.println(lcSample(&lcAmp1));
    delay(100);
  }
  return 0;
}




