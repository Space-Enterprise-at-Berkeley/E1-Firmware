#include <Arduino.h>

#include <Wire.h>
#include <FDC2214.h>
#include <TMP236.h>

#include <CircularBuffer.h>

#include <Comms.h>
#include <Common.h>

#define STATUS_LED 34
#define TEMP_PIN 1
#define EN_485 20
#define TE_485 19

FDC2214 _capSens;
TMP236 _tempSens = TMP236(TEMP_PIN);

void setup()
{
  Serial.begin(115200);
  Comms::initComms();

  Wire.begin();
  _capSens = FDC2214();
  _capSens.begin(0x2A, &Wire);

  _tempSens.init();

  pinMode(EN_485, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(17, OUTPUT);
}

unsigned long previousMillis = 0;
const long interval = 25;

const uint8_t logSecs = 8;

CircularBuffer<float, (logSecs * 1000 / interval)> capBuffer;

Comms::Packet capPacket = {.id = PACKET_ID};

void loop()
{

  ArduinoOTA.handle();

  digitalWrite(EN_485, HIGH);
  digitalWrite(STATUS_LED, LOW);
  digitalWrite(17, LOW);

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    float capValue = _capSens.readCapacitance();

    capBuffer.push(capValue);

    float maxCap = 0;
    for (int i = 0; i < capBuffer.size(); i++){
      if(maxCap < capBuffer[i]) maxCap = capBuffer[i];
    }

    float tempValue = _tempSens.readTemperature();

    capPacket.len = 0;
    Comms::packetAddFloat(&capPacket, capValue);
    Comms::packetAddFloat(&capPacket, maxCap);
    Comms::packetAddFloat(&capPacket, tempValue);
    Comms::emitPacket(&capPacket);
  }
}
