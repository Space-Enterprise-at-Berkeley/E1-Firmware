
#include <Arduino.h>
#include <SPI.h>
#include <INA219.h>
#include <Wire.h>

const float solenoidMonitorShunt = 0.033; // ohms
const float solenoidMonitorMaxCurrent = 5; // amps

const uint8_t numINA219 = 1;
uint8_t _addrs[numINA219] = {0x44};//, 0x45, 0x46, 0x47, 0x48, 0x49, 0x50};
INA219 powerSupplyMonitors[numINA219];

const int num_channels = 1;
uint8_t in1_pins[num_channels] = {2};//, 4, 6, 11, 24, 28, 37};
uint8_t in2_pins[num_channels] = {3};//, 5, 7, 12, 25, 29, 36};
bool h_forward[num_channels] = {0};//, 0, 0, 0, 0, 0, 0}

int outTimer = 0;
bool active = false;

void setup() {
  Serial.begin(9600);
  delay(1000);

  for (int i = 0; i < numINA219; i++) {
      powerSupplyMonitors[i].begin(&Wire, _addrs[i]);
      powerSupplyMonitors[i].configure(INA219_RANGE_32V, INA219_GAIN_40MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
      powerSupplyMonitors[i].calibrate(solenoidMonitorShunt, solenoidMonitorMaxCurrent);
  }

  for (int i = 0; i < num_channels; i++){
    pinMode(in1_pins[i], OUTPUT);
    pinMode(in2_pins[i], OUTPUT);
  }

  outTimer = millis();
}

void loop() {

  //If off for 2 seconds, turn on.
  if (millis() - outTimer >= 2000 && !active) {
    int i = 0;
    if(h_forward[i]) {
      digitalWrite(in1_pins[i], LOW);
      digitalWrite(in2_pins[i], HIGH);
    } else {
      digitalWrite(in1_pins[i], HIGH);
      digitalWrite(in2_pins[i], LOW);
    }
    h_forward[i] = !h_forward[i];
    active = true;
    outTimer = millis();
  }
  //If on for 3 seconds, turn off
  if (millis() - outTimer >= 6000 && active) {
    int i = 0;
    digitalWrite(in1_pins[i], HIGH);
    digitalWrite(in2_pins[i], HIGH);
    outTimer = millis();
    active = false;
  }

  for (int i = 0; i < numINA219; i ++){
    // Serial.print("INA219 ");
    // Serial.print(i);
    // Serial.print(": V=");
    // Serial.print(powerSupplyMonitors[i].readBusVoltage());
    // Serial.print(" I=");
    Serial.println(powerSupplyMonitors[i].readShuntCurrent());
  }
  delay(20);
  

}
