#include <Arduino.h>
#include <Wire.h> 
#include <MCP9600.h>

MCP9600 tcAmp0;
MCP9600 tcAmp1;
MCP9600 tcAmp2;
MCP9600 tcAmp3;
// Testing MCP96L01: same code

float tcSample(MCP9600 *amp) {
        // read from all TCs in sequence
        return amp->readThermocouple();
    }

int main() {
    Wire.begin();
    tcAmp0.init(0x60, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
    tcAmp1.init(0x67, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
    // tcAmp2.init(0x62, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);
    // tcAmp3.init(0x63, &Wire, MCP9600_ADCRESOLUTION_16, MCP9600_TYPE_K, 0);

  while(1) {
    Serial.print("TC0: ");
    Serial.println(tcSample(&tcAmp0));
    Serial.print("TC1");
    Serial.print(tcSample(&tcAmp1));
    // Serial.print("  TC2: ");
    // Serial.print(tcSample(&tcAmp2));
    // Serial.print("  TC3: ");
    // Serial.println(tcSample(&tcAmp3));
    delay(150);
  }
  return 0;
}
