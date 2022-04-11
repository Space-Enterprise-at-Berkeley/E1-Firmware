#include <SPIFlash.h>    //get it here: https://github.com/LowPowerLab/SPIFlash
#include "Wire.h"
#include "BMP280.h"
#include "MicroNMEA.h"
#include "I2Cdev.h"
#include "MPU6050.h"

#define P0 1013.25
#define SERIAL_BAUD      115200
#define LED_BUILTIN 2
#define SS_FLASHMEM -1
#define SEALEVELPRESSURE_HPA (1013.25)
#define BMP_CS 22

#define GPS_FREQUENCY 5
#define BAROMETER_FREQUENCY 50
#define ACCELEROMETER_FREQUENCY 200


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

char bmpString[100];
double Te, Pr, Al;

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
}
void eraseFlash() {
  Serial.print("erasing entire flash... ");
  flash.chipErase();
  while (flash.busy());
  Serial.println("done");
}
void initBMP() {
  if (!bmp.begin()) {
    Serial.println("BMP init failed");
  }
  else Serial.println("BMP init success!");
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
    Serial.println("Error.");
  }
  bmp.startMeasurment();
  if (doubleEqualsZero(*Pr) || doubleEqualsZero(*Al) || (*Pr > 100000) || (*Pr < -100000) 
      || (int)((*Te)*(-(double)100)) == 5988 || (int)((*Te)*((double)100)) == 19283) {
    *Pr = *Al = *Te = 0;
  }
}
void initGPS() {
  Serial1.begin(9600, 134217756U, 18, 19);
  GPSString = (char*) malloc(sizeof(char) * 200);
  GPSCtr = 0;

}
void log(char* s) {
  Serial2.printf("Radio log: %s\n", s);
  Serial.printf("Serial log: %s\n", s);
}
void log(String s) {
  Serial2.printf("Radio log: %s", s);
  Serial.printf("Serial log: %s", s);
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
void doGPS() {
  GPSToNMEA(); 
  log(NMEAString); 
}
void doMPU() {
   mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
   Serial2.printf("Radio log: ax: %d, ay: %d, az: %d, gx: %d, gy: %d, gz %d\n", ax, ay, az, gx, gy, gz);
   Serial.printf("Serial log: ax: %d, ay: %d, az: %d, gx: %d, gy: %d, gz %d\n", ax, ay, az, gx, gy, gz);
 
}
void doBMP() {
  readBMP(&Te, &Pr, &Al);
  Serial.printf("Serial log: t: %.2f, p: %.2f, a: %.2f\n", Te, Pr, Al);
  Serial2.printf("Radio log: t: %.2f, p: %.2f, a: %.2f\n", Te, Pr, Al);
}
void setup() {
  Serial.begin(115200); //set up serial over usb
  Serial2.begin(57600); // set up radio
  initBMP();
  initGPS();
  //initFlash();
  // Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}
void loop() {

  if (Serial1.available()) { //scan for GPS input
    SerialEvent1();
  }
  doGPS();
  doMPU();
  doBMP();
  delay(100);

}