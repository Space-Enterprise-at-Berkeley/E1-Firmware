
#include <Arduino.h>

#include <Wire.h>
#include <FDC2214.h>
#include <TMP236.h>

#include <CircularBuffer.h>

#include <Comms.h>
#include <Common.h>

#define LOX_SERIAL Serial // TODO: verify this
#define LOX_REN_PIN 39
// #define FUEL_SERIAL Serial1 // TODO: verify this
// #define FUEL_REN_PIN 34

FDC2214 _capSens;

void setup()
{
  LOX_SERIAL.begin(115200);
//   FUEL_SERIAL.begin(115200);
  
  // Comms::initComms();

  Wire.begin();
  _capSens = FDC2214();
  _capSens.begin(0x2A, &Wire);
  pinMode(39, OUTPUT);
  pinMode(34, OUTPUT);
  digitalWrite(39, LOW);
  digitalWrite(34, LOW);

}

unsigned long previousMillis = 0;
const long interval = 25;

const uint8_t logSecs = 5;

CircularBuffer<float, (logSecs * 1000 / interval)> capBuffer;

Comms::Packet capPacket = {.id = PACKET_ID};

char loxBuffer[sizeof(Comms::Packet)];
int loxCnt = 0;

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    float capValue = 0.1;

    capBuffer.push(capValue);

    float avgCap = 0;
    for (int i = 0; i < capBuffer.size(); i++){
      avgCap += capBuffer[i];
    }
    avgCap /= capBuffer.size();

    float tempValue = 0.1;

   //LOX_SERIAL.println(capValue);

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


    
    while(LOX_SERIAL.available()) {
          loxBuffer[loxCnt] = LOX_SERIAL.read();
          DEBUG((uint8_t)loxBuffer[loxCnt]);
          DEBUG_FLUSH();
          if(loxCnt == 0 && loxBuffer[loxCnt] != 221) {
              break;
          }
          if(loxBuffer[loxCnt] == '\n') {
              digitalWrite(LOX_REN_PIN, HIGH);
              Comms::Packet *packet = (Comms::Packet *)&loxBuffer;
              Comms::Packet sensorData = {.id=221};
              if(Comms::verifyPacket(packet)) {
                  DEBUG(packet->id);
                  DEBUG_FLUSH();
                  loxCnt = 0;
                  if(packet->id == 221) {
                    sensorData.data[0] = 0xaa;
                    sensorData.len = 1;
                    Comms::emitPacket(&sensorData, &LOX_SERIAL);
                    digitalWrite(LOX_REN_PIN, LOW);
                  }
                  break;
              }
          }
          loxCnt++;
    }
    // Send over serial, but disable if in debug mode
    

  }
}
