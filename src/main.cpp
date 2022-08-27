
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
}

unsigned long previousMillis = 0;
const long interval = 25;

const uint8_t logSecs = 5;

CircularBuffer<float, (logSecs * 1000 / interval)> capBuffer;

Comms::Packet capPacket = {.id = PACKET_ID};

void loop()
{
  digitalWrite(EN_485, HIGH);
  digitalWrite(STATUS_LED, LOW);

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    float capValue = _capSens.readCapacitance();

    capBuffer.push(capValue);

    float avgCap = 0;
    for (int i = 0; i < capBuffer.size(); i++){
      avgCap += capBuffer[i];
    }
    avgCap /= capBuffer.size();

    float tempValue = _tempSens.readTemperature();

    Serial.println(capValue);

    capPacket.len = 0;
    Comms::packetAddFloat(&capPacket, capValue);
    Comms::packetAddFloat(&capPacket, avgCap);
    Comms::packetAddFloat(&capPacket, tempValue);
    Comms::emitPacket(&capPacket);
  }
}
