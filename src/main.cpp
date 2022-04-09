#include <SPIFlash.h>    //get it here: https://github.com/LowPowerLab/SPIFlash
#include "Wire.h"
#include <Adafruit_BMP3XX.h>
#include <Adafruit_BMP280.h>
#include "BMP280.h"

#define P0 1013.25
#define SERIAL_BAUD      115200
#define LED_BUILTIN 2
#define SS_FLASHMEM -1
#define SEALEVELPRESSURE_HPA (1013.25)
#define BMP_CS 22


uint16_t expectedDeviceID=0xEF40;
SPIFlash flash(SS_FLASHMEM, expectedDeviceID);
BMP280 bmp;

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
  bmp.setOversampling(6);
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
}

void setup()
{
  Serial.begin(115200);

  initBMP();
  
  
}
void loop()
{
  double T, P, A;

  readBMP(&T, &P, &A);
  Serial.printf("%.2f, %.2f, %.2f\n", T, P, A);
  
  delay(20);
}