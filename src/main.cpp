#include <Arduino.h>

#include <Wire.h>
#include <FDC2214.h>

#include <Comms.h>
#include <Common.h>
 
FDC2214 _capSens;

void setup()
{
  Comms::initComms();
  Serial.begin(115200);

  Wire.begin();
  _capSens = FDC2214();
  _capSens.begin(0x2A, &Wire);

  pinMode(32, OUTPUT);
}
 
unsigned long previousMillis = 0;  
const long interval = 50;

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

    //Serial.println(capValue);
    Comms::packetAddFloat(&capPacket, capValue);
    Comms::emitPacket(&capPacket);
  }
  digitalWrite(32, LOW);
}
