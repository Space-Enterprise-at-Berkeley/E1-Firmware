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

Comms::Packet capPacket = {.id = 5};

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  //Comms::initComms();

  pinMode(EN_485, OUTPUT);
  pinMode(TE_485, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  #ifdef LOX
  digitalWrite(EN_485, HIGH);
  #else
  digitalWrite(EN_485, LOW);
  #endif
  digitalWrite(TE_485, HIGH);
  
  Comms::packetAddFloat(&capPacket, 69);
  Comms::packetAddFloat(&capPacket, 69);
  Comms::packetAddFloat(&capPacket, 69);
}
unsigned long previousMillis = 0;
const long interval = 25;

const uint8_t logSecs = 5;

CircularBuffer<float, (logSecs * 1000 / interval)> capBuffer;

int count = 0;

void loop()
{
  #ifdef LOX
  Comms::emitPacket(&capPacket);
  delay(6);
  #else
  if (Serial1.available() > 0) {
    // read the incoming byte:
    char end = '\n';
    String incomingByte = Serial1.readStringUntil(end);

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte);
  }
  delay(50);
  #endif
}
