#include <Arduino.h>
#include <Wire.h> 
#include <BMP388_DEV.h>

BMP388_DEV bmp388;
    
float baroAltitude, baroPressure, baroTemperature;

uint32_t sampleAltPressTemp() {
    Serial.println("BAROMETER SAMPLING!!!!");
    bmp388.startForcedConversion(); // Start a forced conversion (if in SLEEP_MODE)
    Serial.println("Starting measuring");
    bmp388.getMeasurements(baroTemperature, baroPressure, baroAltitude);
    return 0;
}


int main() {
    Serial.begin(115200);
    while (!Serial) { 

    }

    Serial.println("Starting");
    Wire.begin();
    Wire.setClock(100000);
    bmp388.begin(0x76);
    Serial.println("Barometer INIT");

  while(1) {
    sampleAltPressTemp();
    Serial.print("Barometer ALTITUDE: ");
    Serial.println(baroAltitude);
    Serial.print("Barometer PRESSURE: ");
    Serial.println(baroPressure);
    Serial.print("Barometer TEMPERATURE: ");
    Serial.println(baroTemperature);
    delay(150);
  }
  return 0;
}
