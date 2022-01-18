#include <Arduino.h>

#include <Wire.h>
#include "FDC2214.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Comms.h>
 
FDC2214 _capSens;
uint16_t Fletcher16(uint8_t *data, int count) {

  uint16_t sum1 = 0;
  uint16_t sum2 = 0;

  for (int index=0; index<count; index++) {
    if (data[index] > 0) {
      sum1 = (sum1 + data[index]) % 256;
      sum2 = (sum2 + sum1) % 256;
    }
  }
  return (sum2 << 8) | sum1;
}

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

uint8_t packet[12];

void loop()
{
  digitalWrite(32, HIGH);
  ArduinoOTA.handle();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    float cap = _capSens.readCapacitance();

    uint8_t check_in[10];

    check_in[0] = 220;
    check_in[1] = 4;

    
    long timestamp = millis();

    uint32_t rawTime = * ( uint32_t * ) &timestamp;

    check_in[2] = rawTime & 0xFF;
    check_in[3] = rawTime >> 8 & 0xFF;
    check_in[4] = rawTime >> 16 & 0xFF;
    check_in[5] = rawTime >> 24 & 0xFF;

    uint32_t rawData = * ( uint32_t * ) &cap;

    check_in[6] = rawData & 0xFF;
    check_in[7] = rawData >> 8 & 0xFF;
    check_in[8] = rawData >> 16 & 0xFF;
    check_in[9] = rawData >> 24 & 0xFF;

    // String packet_content = "8,";
    // packet_content += (String) cap;
    // packet_content += ",0,0,0";

    // char const *data = packet_content.c_str();

    uint16_t checksum = Fletcher16((uint8_t *) &check_in, 10);

    // packet_content += "|";
    // String check_ = String(checksum, HEX);
    // while(check_.length() < 4) {
    //   check_ = "0" + check_;
    // }
    // packet_content += check_;
    // String packet = "{" + packet_content + "}";

    packet[0] = check_in[0];
    packet[1] = check_in[1];
    packet[2] = check_in[2];
    packet[3] = check_in[3];
    packet[4] = check_in[4];
    packet[5] = check_in[5];

    packet[6] = checksum & 0xFF;
    packet[7] = checksum >> 8 & 0xFF;    

    packet[8] = check_in[6];
    packet[9] = check_in[7];
    packet[10] = check_in[8];
    packet[11] = check_in[9];

    Udp.beginPacket(ground, 42069); 
    Udp.write((uint8_t*) packet, 12);
    Udp.endPacket();

    for(int i = 0; i<12;i++){
      Serial.print(packet[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
  digitalWrite(32, LOW);
}
