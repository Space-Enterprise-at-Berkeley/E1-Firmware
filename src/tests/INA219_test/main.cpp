
#include <Arduino.h>
#include <SPI.h>
#include <INA219.h>
#include <Wire.h>

const uint8_t numINA219 = 1;
uint8_t _addrs[numINA219] = {0x4A};
INA219 powerSupplyMonitors[numINA219];

const float solenoidMonitorShunt = 0.033; // ohms
const float solenoidMonitorMaxCurrent = 5; // amps

void setup() {
  Serial.begin(9600);
  delay(1000);

  for (int i = 0; i < numINA219; i++) {
      powerSupplyMonitors[i].begin(&Wire1, _addrs[i]);
      powerSupplyMonitors[i].configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
      powerSupplyMonitors[i].calibrate(solenoidMonitorShunt, solenoidMonitorMaxCurrent);
  }
}

void loop() {
  for (int i = 0; i < numINA219; i ++){
    Serial.print("INA219 ");
    Serial.print(i);
    Serial.print(": V=");
    Serial.print(powerSupplyMonitors[i].readBusVoltage());
    Serial.print("; I=");
    Serial.println(powerSupplyMonitors[i].readShuntCurrent());
  }
  delay(1000);
}
