
#include <Arduino.h>
#include <SPI.h>
#include <INA219.h>
#include <Wire.h>

const uint8_t numINA219 = 15;
uint8_t _addrs[numINA219] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
                             0x49, 0x50, 0x51, 0x52, 0x53, 0x54};
INA219 powerSupplyMonitors[numINA219];

const float powerSupplyMonitorShuntR = 0.010; // ohms
const float powerSupplyMonitorMaxExpectedCurrent = 5; // amps

void setup() {
  Serial.begin(9600);
  delay(1000);

  for (int i = 0; i < numINA219; i++) {
      powerSupplyMonitors[i].begin(&Wire1, _addrs[i]);
      powerSupplyMonitors[i].configure(INA219_RANGE_16V, INA219_GAIN_40MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
      powerSupplyMonitors[i].calibrate(powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent);
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
  delay(10);
}
