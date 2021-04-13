
#include <Arduino.h>
#include <SPI.h>
#include <batteryMonitor.h>
#include <Wire.h>


uint8_t battMonINAAddr = 0x40;
const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps

void setup() {
  Serial.begin(9600);
  delay(1000);

  batteryMonitor::init(&Wire, batteryMonitorShuntR, batteryMonitorMaxExpectedCurrent, battMonINAAddr);
}

float data[5];

void loop() {
  batteryMonitor::readAllBatteryStats(data);
    Serial.print("INA226 ");
    Serial.print(": V=");
    Serial.print(data[0]);
    Serial.print("; I=");
    Serial.println(data[2]);
  delay(1000);
}
