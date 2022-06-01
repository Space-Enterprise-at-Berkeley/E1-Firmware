#include <SPIFlash.h>    //get it here: https://github.com/LowPowerLab/SPIFlash
#include "Wire.h"
#include "BMP280.h"
#include "MicroNMEA.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Apogee.h"
#include "Comms.h"
using namespace Comms;
#include "Common.h"
#define P0 1013.25
#define SERIAL_BAUD      115200
#define LED_BUILTIN 2
#define SS_FLASHMEM   25
#define SEALEVELPRESSURE_HPA (1013.25)
#define BMP_CS 22

#define GPS_FREQUENCY 5
#define BAROMETER_FREQUENCY 30
#define ACCELEROMETER_FREQUENCY 30
#define BREAKWIRE_FREQUENCY 30
#define APOGEE_FREQUENCY 10  //10hz, so every 100ms
#define MAX_FLASH_CAPACITY 15990000
#define RECORDING_METADATA_FREQUENCY 1

int validPackets[] = {154, 155};
int numValidPackets = 2;
#define COMMAND_PACKET_LENGTH 8

uint16_t expectedDeviceID=0xEF40;
SPIFlash flash(SS_FLASHMEM, expectedDeviceID);
BMP280 bmp;
char* GPSString;
int GPSCtr;
char NMEAbuffer[100];
MicroNMEA nmea(NMEAbuffer, sizeof(NMEAbuffer));
char NMEAString[100];
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
char MPUString[50];
int serialCommandCountdown;
int commandBuffer[COMMAND_PACKET_LENGTH];
uint32_t cumBytes = 0;
bool dirtyFlash = true;
bool recording = false;
uint8_t packetStoreBuffer[250];

uint32_t lastBaroRead, lastAccelRead, lastGPSRead, lastBreakwireRead, lastSerialCheck, lastRecordingRead, lastApogeeRead = 0;

Packet MPU;
Packet BMP;
Packet GPS;
Packet BW;
Packet Gyro;
Packet RRP;
Packet AP;

char bmpString[100];
double Te, Pr, Al;

void initMPU() {
  mpu.initialize();
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);

  // mpu.setXAccelOffset(-5700);
  // mpu.setYAccelOffset(-6020);
  // mpu.setZAccelOffset(8050);
  mpu.setXAccelOffset(-3790);
  mpu.setYAccelOffset(460);
  mpu.setZAccelOffset(920);
  mpu.setXGyroOffset(60);
  mpu.setXGyroOffset(60);
  mpu.setYGyroOffset(-93);
  mpu.setZGyroOffset(1);
}

void initFlash() {
  pinMode(SS, OUTPUT); digitalWrite(SS, HIGH);

  if (flash.initialize())
  {
    Serial.println("Init OK!");
  }
  else {
    Serial.print("Init FAIL, expectedDeviceID(0x");
    Serial.print(expectedDeviceID, HEX);
    Serial.print(") mismatched the read value: 0x");
    Serial.println(flash.readDeviceId(), HEX);
  }
  dirtyFlash = true;
   
}
void eraseFlash() {
  recording = false;
  //Serial.print("erasing entire flash... ");
  flash.chipErase();
  while (flash.busy());
  //Serial.println("done");
  dirtyFlash = false;
}
void startBlackboxRecord() {
  if (!dirtyFlash) {
    //Serial.print("started recording!");
    dirtyFlash = true;
    recording = true;
  }
}
void stopBlackboxRecord() {
  recording = false;
}
void saveToFlash(uint8_t* buf, int len) {
  if ((!recording) || cumBytes > MAX_FLASH_CAPACITY) return;

  for (int i = 0; i < len; i++) {
    //Serial.printf("%x, ", buf[i]);
    flash.writeBytes(cumBytes, buf, len);
    while (flash.busy()) {}
  }
  //Serial.println();
  cumBytes += len;
}
void initBMP() {
  if (!bmp.begin()) {
    //Serial.println("BMP init failed");
  }
  else //Serial.println("BMP init success!");
  bmp.setOversampling(2);
}
boolean doubleEqualsZero(double f) {
  double e = 0.01;
  if (abs(f) < e) return true;
  return false;
}
void readBMP(double* Te, double *Pr, double *Al) {
  double T,P;
  int result = bmp.getTemperatureAndPressure(T,P);
  if(result!=0) {
    double A = bmp.altitude(P,P0);
    *Al = A;
    *Pr = P;
    *Te = T;
  } else {
    *Al = *Pr = *Te = 0;
    //Serial.println("Error.");
  }
  bmp.startMeasurment();
  if (doubleEqualsZero(*Pr) || doubleEqualsZero(*Al) || (*Pr > 100000) || (*Pr < -100000) 
      || (int)((*Te)*(-(double)100)) == 5988 || (int)((*Te)*((double)100)) == 19283) {
    *Pr = *Al = *Te = 0;
  }
}

void initGPS() {
  Serial1.begin(38400, SERIAL_8N1, 19, 18);
  GPSString = (char*) malloc(sizeof(char) * 200);
  GPSCtr = 0;

  GPS.id = 6;
  GPS.len = 0;

  packetAddFloat(&GPS, 27.9881f);
  packetAddFloat(&GPS, 86.9250f);
  packetAddFloat(&GPS, 69.0f);
  packetAddFloat(&GPS, 420.0f);
  packetAddUint8(&GPS, 0);
  packetAddUint8(&GPS, 0);

}


void log(char* s) {
  Serial2.printf("Radio log: %s\n", s);
  ////Serial.printf("Serial log: %s\n", s);
}
void log(String s) {
  Serial2.println("Radio log: " + s);
  ////Serial.println("Serial log: " + s);
}
void processGPSLine() {
  GPSString[GPSCtr] = 0;
  GPSCtr = 0;
}
void SerialEvent1() {
  char f = Serial1.read();
  nmea.process(f);
  if (f == '\n') {
    processGPSLine();
  } else {
    GPSString[GPSCtr] = f;
    GPSCtr ++;
  }
  if (Serial1.available()) {
    SerialEvent1();
  }
}
void GPSToNMEA() {
    long alt = 0;
    nmea.getAltitude(alt);
    sprintf(NMEAString, "%d %c %d %.2f %d-%d %d:%d:%d LA%.6f LO%.6f AL%.4f SP%.4f C%.4f\n", 
          nmea.isValid() ? 1 : 0, nmea.getNavSystem(), nmea.getNumSatellites(), (float) nmea.getHDOP() / 10.0f,
          (uint8_t) nmea.getMonth(), (uint8_t) nmea.getDay(), (uint8_t) nmea.getHour(), 
          (uint8_t) nmea.getMinute(), (uint8_t) nmea.getSecond(),
          (float) nmea.getLatitude() /1000000.0f, (float) nmea.getLongitude() / 1000000.0f, alt/1000.0f,
          (float) nmea.getSpeed() / 1000.0f, (float)nmea.getCourse() / 1000.0f);
}

void doGPS(Packet* f) {
  if (nmea.isValid()) {
    f->id = 6;
    f->len = 0;
    packetAddFloat(f, (float) nmea.getLatitude() / 1000000.0f);
    packetAddFloat(f, (float) nmea.getLongitude() / 1000000.0f);
    long alt;
    nmea.getAltitude(alt);
    packetAddFloat(f, (float) alt/1000.0f);
    packetAddFloat(f, (float) nmea.getSpeed() / 1000.0f);
    packetAddFloat(f, (float) nmea.getCourse() / 1000.0f);
    packetAddUint8(f, nmea.isValid() ? 1 : 0);
    packetAddUint8(f, nmea.getNumSatellites());
  } 
  int len = emitPacket(f, packetStoreBuffer);
  saveToFlash(packetStoreBuffer, len);
  //GPSToNMEA(); 
  //log(NMEAString); 
}

void doBW(Packet* f) {
  f->id = 39;
  f->len = 0;
  packetAddFloat(f, (3.3 * (float)analogRead(36))/4096);
  //packetAddFloat(f, 0.5);
  packetAddFloat(f, (3.3 * (float)analogRead(39))/4096);
  int len = emitPacket(f, packetStoreBuffer);
  saveToFlash(packetStoreBuffer, len);
}

void doGyro(Packet* f) {
  f->id = 57;
  f->len = 0;
  packetAddFloat(f, ((float) gx)/16.384f);
  packetAddFloat(f, ((float) gy)/16.384f);
  packetAddFloat(f, ((float) gz)/16.384f);
  int len = emitPacket(f, packetStoreBuffer);
  saveToFlash(packetStoreBuffer, len);
}

void doMPU(Packet* f) {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  

  //accel
  f->id = 4;
  f->len = 0;
  packetAddFloat(f, (float) 0);
  packetAddFloat(f, (float) 0);
  packetAddFloat(f, (float) 0);
  packetAddFloat(f, (float) 0);
  packetAddFloat(f, ((float) ax) / 2048.0f);
  packetAddFloat(f, ((float) ay) / 2048.0f);
  packetAddFloat(f, ((float) az) / 2048.0f);
  // packetAddFloat(f, ((float) ax) / 1.0f);
  // packetAddFloat(f, ((float) ay) / 1.0f);
  // packetAddFloat(f, ((float) az) / 1.0f);

  int len = emitPacket(f, packetStoreBuffer);
  saveToFlash(packetStoreBuffer, len);

  doGyro(&Gyro);

}


void doBMP(Packet* f) {
  readBMP(&Te, &Pr, &Al);
  f->id = 5;
  f->len = 0;
  packetAddFloat(f, (float) Al);
  packetAddFloat(f, (float) Pr);
  packetAddFloat(f, (float) Te);
  int len = emitPacket(f, packetStoreBuffer);
  saveToFlash(packetStoreBuffer, len);

  //char buffer[60];
  //sprintf(buffer, "t: %.2f, p: %.2f, a: %.2f", Te, Pr, Al);
  //log(String(buffer));
}

void doSerial() {
  int f = Serial2.available();
  //Serial.println(f);
  for (int i = 0; i < f; i++) {
    Serial2.read();
  }
}

void doRecordingRead(Packet* f) {
  f->id = 84;
  f->len = 0;
  packetAddUint8(f, recording ? 1 : 0);
  packetAddUint8(f, dirtyFlash ? 0 : 1);
  packetAddUint32(f, cumBytes / 1000);
  emitPacket(f, packetStoreBuffer);
}

void doApogee(Packet* f) {
  f->id = 158;
  Apogee::apogeeDetectionTask(f, (float) Al);

  int len = emitPacket(f, packetStoreBuffer);
  saveToFlash(packetStoreBuffer, len);

}

Packet g;
uint32_t lastSerialMsg;
char serialBuffer[8];
int serialBufferPtr;
void setup() {
  delay(1000);
  Serial.begin(115200); //set up serial over usb
  Serial2.begin(115200); // set up radio
  initBMP();
  initGPS();
  initFlash();
  initMPU();
  pinMode(36, PULLDOWN); //breakwire pins, idt it does anything though
  pinMode(39, PULLDOWN);
  Serial.println("hi  i work");
  ////Serial.println("bruh");
  for (int i = 0; i < 200; i+=20) {
    for (int j = i; j < i+20; j++) {
      Serial.printf("%x ", flash.readByte(j));
    }
    Serial.println();
  }

}

void loop() {

  if (Serial1.available()) { //scan for GPS input
    SerialEvent1();
  }
// MAKE SERIAL2 OR WHATEVER FOR RADIO IN
  if (Serial.available() > 8) {
    char byte1 = Serial.read();
    char byte2 = Serial.peek();
    if (byte1 == 105 && ((byte2 == 155)||(byte2 == 154))) {

      Serial.readBytes((uint8_t*) &serialBuffer, 8);
      //////Serial.printf("got packet! %x..%x\n", serialBuffer[0], serialBuffer[7]);
      Packet* p = (Packet*)&serialBuffer;
      uint16_t checksum = *(uint16_t *)&p->checksum;
      if (checksum == computePacketChecksum(p)) {
        if (p->id == 155) {
          eraseFlash();
        } else if (p->id == 154) {
          startBlackboxRecord();
        } else {
          //Serial.println("wtf");
        }
      }
    }
  }


  if ((micros() - lastAccelRead) > (1000000 / ACCELEROMETER_FREQUENCY)) {
    lastAccelRead = micros();
    doMPU(&MPU);
  }
  if ((micros() - lastRecordingRead) > (1000000 / RECORDING_METADATA_FREQUENCY)) {
    lastRecordingRead = micros();
    doRecordingRead(&RRP);
  }
  if ((micros() - lastGPSRead) > (1000000 / GPS_FREQUENCY)) {
    lastGPSRead = micros();
    doGPS(&GPS);
  }
  if ((micros() - lastBaroRead) > (1000000 / BAROMETER_FREQUENCY)) {
    lastBaroRead = micros();
    doBMP(&BMP);
  }

  if ((micros() - lastBreakwireRead) > (1000000 / BREAKWIRE_FREQUENCY)) {
    lastBreakwireRead = micros();
    doBW(&BW);
  }

  if ((micros() - lastApogeeRead) > (1000000 / APOGEE_FREQUENCY)) {
    lastApogeeRead = micros();
    doApogee(&AP);
  }

}