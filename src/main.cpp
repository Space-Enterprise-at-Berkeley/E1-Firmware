#include <Arduino.h>

#include <Wire.h>
#include "FDC2214.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Comms.h>
#include <Common.h>
 
FDC2214 _capSens;

void setup()
{
  pinMode(32, OUTPUT);
  Wire.begin();
  _capSens = FDC2214();
  _capSens.begin(0x2A, &Wire);
  Serial.begin(115200);

  Comms::initComms();
}
 
unsigned long previousMillis = 0;  
const long interval = 50;
int last = 0;

Comms::Packet capPacket = {.id = 220};

void loop()
{
  digitalWrite(32, HIGH);
  ArduinoOTA.handle();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    float capValue = _capSens.readCapacitance();

    capPacket.len = 0;
    Comms::packetAddFloat(&capPacket, capValue);
    Comms::emitPacket(&capPacket);
  }
  digitalWrite(32, LOW);
}
