
#include <Arduino.h>

#include <Wire.h>
#include <FDC2214.h>
#include <TMP236.h>

#include <CircularBuffer.h>

#include <Comms.h>
#include <Common.h>

#define STATUS_LED 34
#define TEMP_PIN 1
#define EN_485 20 // switch between transmit and receive
#define TE_485 19 // terminate enable

FDC2214 _capSens;
TMP236 _tempSens = TMP236(TEMP_PIN);

char rs485Buffer[sizeof(Comms::Packet)];
int cnt = 0;

void setup()
{
  Serial.begin(115200);
  Serial1.begin(921600);

  Wire.begin();
  _capSens = FDC2214();
  _capSens.begin(0x2A, &Wire);

  _tempSens.init();

  pinMode(EN_485, OUTPUT);
  pinMode(TE_485, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  digitalWrite(EN_485, LOW); // put in receive mode by default
  #ifdef FUEL
  digitalWrite(TE_485, HIGH);
  #else
  digitalWrite(TE_485, LOW);
  #endif
  digitalWrite(STATUS_LED, LOW);
}

unsigned long previousMillis = 0;
const long interval = 25;

const uint8_t logSecs = 5;

CircularBuffer<float, (logSecs * 1000 / interval)> capBuffer;

Comms::Packet capPacket = {.id = PACKET_ID};

void loop()
{
  while(Serial1.available()) {
    rs485Buffer[cnt] = Serial1.read();
    // DEBUG((uint8_t)rs485Buffer[cnt]);
    // DEBUG(" ");
    // DEBUG_FLUSH();
    if(cnt == 0 && rs485Buffer[cnt] != PACKET_ID) {
      break;
    }

    if(rs485Buffer[cnt] == '\n') {
      Comms::Packet *packet = (Comms::Packet *)&rs485Buffer;
      if(Comms::verifyPacket(packet)) {
        digitalWrite(STATUS_LED, HIGH);
        cnt = 0;

        // Comms::emitPacket(&capPacket, &Serial);
        digitalWrite(EN_485, HIGH);
        Comms::emitPacket(&capPacket, &Serial1);
        Serial1.flush();
        digitalWrite(EN_485, LOW);
        digitalWrite(STATUS_LED, LOW);
        break;
      }
    }
    cnt++;

    if(cnt >= 20) {
      cnt = 0;
    }
  }




  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    float capValue = _capSens.readCapacitance();
    DEBUG(capValue);
    DEBUG('\n');
    DEBUG_FLUSH();

    capBuffer.push(capValue);

    float avgCap = 0;
    for (int i = 0; i < capBuffer.size(); i++){
      avgCap += capBuffer[i];
    }
    avgCap /= capBuffer.size();

    float tempValue = _tempSens.readTemperature();

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
  }
}
