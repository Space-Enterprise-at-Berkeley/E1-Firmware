
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
int indicatorDuty = 500;
int indicatorPeriod = 1000;
int indicatorLastTime = 0;

const int timeBetweenTransmission = 100; // ms
int lastTransmissionTime = 0;

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

  #ifdef FUEL
  delay(50);
  #endif
}

unsigned long previousMillis = 0;
const long interval = 25;

const uint8_t logSecs = 5;

CircularBuffer<float, (logSecs * 1000 / interval)> capBuffer;

Comms::Packet capPacket = {.id = PACKET_ID};

void loop()
{
  // while(Serial1.available()) {
  //   rs485Buffer[cnt] = Serial1.read();
  //   // DEBUG((uint8_t)rs485Buffer[cnt]);
  //   // DEBUG(" ");
  //   // DEBUG_FLUSH();
  //   if(cnt == 0 && rs485Buffer[cnt] != PACKET_ID) {
  //     break;
  //   }

  //   if(rs485Buffer[cnt] == '\n') {
  //     Comms::Packet *packet = (Comms::Packet *)&rs485Buffer;
  //     if(Comms::verifyPacket(packet)) {
  //       cnt = 0;

  //       // Comms::emitPacket(&capPacket, &Serial);
  //       digitalWrite(EN_485, HIGH);
  //       Comms::emitPacket(&capPacket, &Serial1);
  //       Serial1.flush();
  //       digitalWrite(EN_485, LOW);
  //       break;
  //     }
  //   }
  //   cnt++;

  //   if(cnt >= 20) {
  //     cnt = 0;
  //   }
  // }
  if(millis() - lastTransmissionTime >= timeBetweenTransmission) {
    DEBUG("Transmitting ");
    DEBUG(Comms::packetGetFloat(&capPacket, 0));
    DEBUG("\n");
    DEBUG_FLUSH();
    lastTransmissionTime = lastTransmissionTime + timeBetweenTransmission;
    digitalWrite(EN_485, HIGH);
    Comms::emitPacket(&capPacket, &Serial1);
    Serial1.flush();
    digitalWrite(EN_485, LOW);
  }

  #ifdef FUEL
  if(Serial1.available() && millis() - lastTransmissionTime >= timeBetweenTransmission/4) {
    DEBUG("R\n");
    lastTransmissionTime = millis() - timeBetweenTransmission/2;
    while(Serial1.available()) {
      Serial1.read();
    }
  }
  #endif




  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    float capValue = _capSens.readCapacitance();
    if(capValue < 0.0) {
      // error reading from sensor
      indicatorDuty = 200;
    } else {
      indicatorDuty = 500;
    }
    DEBUG(capValue);
    DEBUG('\n');
    DEBUG_FLUSH();

    capBuffer.push(capValue);

    float capValue1 = _capSens.readCapacitance1();

    float avgCap = 0;
    for (int i = 0; i < capBuffer.size(); i++){
      avgCap += capBuffer[i];
    }
    avgCap /= capBuffer.size();

    float tempValue = _tempSens.readTemperature();

    Serial.println(tempValue);
    Serial.println(capValue);
    Serial.println(_capSens.readSensor(1));
    Serial.println();

    capPacket.len = 0;
    Comms::packetAddFloat(&capPacket, capValue);
    Comms::packetAddFloat(&capPacket, capValue1);
    Comms::packetAddFloat(&capPacket, avgCap);
    Comms::packetAddFloat(&capPacket, tempValue);
    // Comms::packetAddFloat(&capPacket, 0.0);
    
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

  int timeNow = currentMillis = millis();
  if(timeNow - indicatorLastTime >= indicatorDuty) {
    digitalWrite(STATUS_LED, HIGH);
  }
  if(timeNow - indicatorLastTime >= indicatorPeriod) {
    digitalWrite(STATUS_LED, LOW);
    indicatorLastTime = timeNow;
  }
}
