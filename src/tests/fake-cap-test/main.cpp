
#include <Arduino.h>

#include <Wire.h>

#include <CircularBuffer.h>

#include <Comms.h>
#include <Common.h>

// #define STATUS_LED 34
// #define TEMP_PIN 1
// #define EN_485 20
// #define TE_485 19
#define DI_RS485 19
#define RO_RS485 20

FDC2214 _capSens;
TMP236 _tempSens = TMP236(TEMP_PIN);

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  
  // Comms::initComms();

  Wire.begin();
  _capSens = FDC2214();
  _capSens.begin(0x2A, &Wire);

  _tempSens.init();

  pinMode(EN_485, OUTPUT);
  pinMode(TE_485, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  digitalWrite(EN_485, HIGH);
  digitalWrite(TE_485, HIGH);
  digitalWrite(STATUS_LED, LOW);
}

unsigned long previousMillis = 0;
const long interval = 25;

const uint8_t logSecs = 5;

CircularBuffer<float, (logSecs * 1000 / interval)> capBuffer;

Comms::Packet capPacket = {.id = PACKET_ID};

void loop()
{
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
    
    uint32_t timestamp = millis();
    capPacket.timestamp[0] = timestamp & 0xFF;
    capPacket.timestamp[1] = (timestamp >> 8) & 0xFF;
    capPacket.timestamp[2] = (timestamp >> 16) & 0xFF;
    capPacket.timestamp[3] = (timestamp >> 24) & 0xFF;

    //calculate and append checksum to struct
    uint16_t checksum = Comms::computePacketChecksum(&capPacket);
    capPacket.checksum[0] = checksum & 0xFF;
    capPacket.checksum[1] = checksum >> 8;

    // Send over serial, but disable if in debug mode
    Serial.write(capPacket.id);
    Serial.write(capPacket.len);
    Serial.write(capPacket.timestamp, 4);
    Serial.write(capPacket.checksum, 2);
    Serial.write(capPacket.data, capPacket.len);
    Serial.write('\n');
    Serial.flush();

    Serial1.write(capPacket.id);
    Serial1.write(capPacket.len);
    Serial1.write(capPacket.timestamp, 4);
    Serial1.write(capPacket.checksum, 2);
    Serial1.write(capPacket.data, capPacket.len);
    Serial1.write('\n');
    Serial1.flush();
  }
}