
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

// samhitag3 added variables for maintaining running average
int numSamples = 0;
int oldestSampleIndex = 0;
int const sampleSize = 100;
float samples[sampleSize];
float total = 0;
float baseline = 0;

const int timeBetweenTransmission = 100; // ms
int lastTransmissionTime = 0;

//samhitag3 added runningAverage method
float runningAverage(float total, int numSamples){
  if (numSamples == 0) {
    return 0;
  }
  return total / numSamples;
}

void setup()
{
  Serial.begin(115200);
  //samhitag3 testing slower baud rate
  // Serial.begin(9600);
  // samhitag3 commented out
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
CircularBuffer<float, (logSecs * 1000 / interval)> refBuffer;
CircularBuffer<float, (logSecs * 1000 / interval)> correctedBuffer;

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
    // samhitag3 changed readCapacitance input value
    float capValue = _capSens.readCapacitance(00);
    // samhitag3 defined refValue, sensor0, sensor1
    float sensor0 = _capSens.readSensor(00);
    float sensor1 = _capSens.readSensor(01);
    float refValue = _capSens.readCapacitance(01);

    // samhitag3 passed data to running average method
    if (numSamples < sampleSize) {
      if (numSamples == 0) {
        baseline = refValue;
      }
      samples[numSamples] = refValue;
      total += refValue;
      if (numSamples == sampleSize - 1) {
        baseline = total / numSamples;
      }
      numSamples++;
    }
    //  else {
    //   float total0 = total;
    //   total -= samples[oldestSampleIndex];
    //   total += refValue;
    //   oldestSampleIndex = (oldestSampleIndex + 1) % sampleSize;
    //   // Serial.print(total0 - total);
    // }
    // float refAvg = runningAverage(total, numSamples);

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
    refBuffer.push(refValue);

    float avgCap = 0;
    float avgRef = 0;
    for (int i = 0; i < capBuffer.size(); i++){
      avgCap += capBuffer[i];
      avgRef += refBuffer[i];
    }
    avgCap /= capBuffer.size();
    avgRef /= refBuffer.size();

    float tempValue = _tempSens.readTemperature();
    float corrected = _capSens.correctedCapacitance(avgRef, baseline);

    correctedBuffer.push(corrected);
    float avgCorrected = 0;
    for (int i = 0; i < correctedBuffer.size(); i++){
      avgCorrected += correctedBuffer[i];
    }
    avgCorrected /= correctedBuffer.size();


    // samhitag3 test print statements
    // Serial.print("temp: ");
    Serial.print(tempValue);
    Serial.print("\t");
    Serial.print(sensor0);
    Serial.print("\t");
    Serial.print(sensor1);
    Serial.print("\t");
    Serial.print(capValue);
    Serial.print("\t");
    Serial.print(refValue);
    Serial.print("\t");
    Serial.print(avgCap);
    Serial.print("\t");
    Serial.print(avgRef);
    Serial.print("\t");
    Serial.print(corrected);
    Serial.print("\t");
    Serial.println(avgCorrected);
    // Serial.print(corrected);
    // Serial.print("  numSamp: ");
    // Serial.print(numSamples);
    // Serial.print("  avg: ");
    // Serial.print(refAvg);
    // Serial.print("  total: ");
    // Serial.print(total);
    // Serial.print("  amt: ");
    // Serial.print(numSamples);
    // Serial.print("  base: ");
    // Serial.print(baseline);
    // Serial.println();

    capPacket.len = 0;
    // samhitag3 changed packet variable from capValue to corrected
    Comms::packetAddFloat(&capPacket, corrected);
    Comms::packetAddFloat(&capPacket, avgCap);
    Comms::packetAddFloat(&capPacket, tempValue);
    // samhitag3 added packet variable refValue
    Comms::packetAddFloat(&capPacket, refValue);
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