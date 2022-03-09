#include <Arduino.h>

#include <Wire.h>
#include <FDC2214.h>
#include <TMP236.h>

#include <Comms.h>
#include <Common.h>
 
#define STATUS_LED 34
#define TEMP_PIN 1
#define EN_485 20
#define TE_485 19

FDC2214 _capSens;
TMP236 _tempSens = TMP236(TEMP_PIN);;

void setup()
{
  Serial.begin(115200);
  Comms::initComms();

  Wire.begin();
  _capSens = FDC2214();
  _capSens.begin(0x2A, &Wire);

  _tempSens.init();

  pinMode(STATUS_LED, OUTPUT);
  pinMode(EN_485, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(17, OUTPUT);
}
 
unsigned long previousMillis = 0;  
const long interval = 25;

Comms::Packet capPacket = {.id = 220};

void loop()
{
  digitalWrite(STATUS_LED, LOW);
  digitalWrite(EN_485, HIGH);
  digitalWrite(17, LOW);


  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    float capValue = _capSens.readCapacitance();
    capPacket.len = 0;

    Serial.println(capValue);

    Comms::packetAddFloat(&capPacket, capValue);
    Comms::emitPacket(&capPacket);
  }
}
