#include <SPI.h> //Needed for SPI to GNSS

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

// #########################################

// Instantiate an instance of the SPI class. 
// Your configuration may be different, depending on the microcontroller you are using!

#define spiPort SPI // This is the SPI port on standard Ardino boards. Comment this line if you want to use a different port.

//SPIClass spiPort (HSPI); // This is the default SPI interface on some ESP32 boards. Uncomment this line if you are using ESP32.

// #########################################

const uint8_t csPin = 38; // On ATmega328 boards, SPI Chip Select is usually pin 10. Change this to match your board.

// #########################################

long lastTime = 0; //Simple local timer. Limits amount of SPI traffic to u-blox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println(F("SparkFun u-blox Example"));

  spiPort.begin(); // begin the SPI port

  //myGNSS.enableDebugging(); // Uncomment this line to see helpful debug messages on Serial

  // Connect to the u-blox module using SPI port, csPin and speed setting
  // ublox devices generally work up to 5MHz. We'll use 4MHz for this example:
  if (myGNSS.begin(spiPort, csPin, 4000000) == false) 
  {
    Serial.println(F("u-blox GNSS not detected on SPI bus. Please check wiring. Freezing."));
    while (1);
  }
  
  //myGNSS.factoryDefault(); delay(5000); // Uncomment this line to reset the module back to its factory defaults

  myGNSS.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX); //gSet the SPI port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
}

void loop()
{
  //Query module only every second. Doing it more often will just cause SPI traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer
    
    long latitude = myGNSS.getLatitude();
    Serial.print(F("Lat: "));
    float fLat = (float) latitude;
    Serial.print(fLat / 10e6);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    float fLong = (float) longitude;
    Serial.print(fLong / 10e6);
    Serial.print(F(" (degrees * 10^-7)"));

    long altitude = myGNSS.getAltitude();
    Serial.print(F(" Alt: "));
    float fAlt = (float) altitude;
    Serial.print(fAlt / 10e3);
    Serial.print(F(" (mm)"));

    long year = myGNSS.getYear();
    Serial.print(F(" Year: "));
    Serial.print(year);

    long month = myGNSS.getMonth();
    Serial.print(F(" Month: "));
    Serial.print(month);

    long day = myGNSS.getDay();
    Serial.print(F(" Day: "));
    Serial.print(day);

    long hour = myGNSS.getHour();
    long minute = myGNSS.getMinute();
    long sec = myGNSS.getSecond();
    Serial.print(F(" Time: "));
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.print(sec);
    

    byte SIV = myGNSS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.print(SIV);

    Serial.println();
  }
}
// #include "GPS.h"
// #include "SPI.h"


// namespace GPS {

//     uint32_t gpsUpdatePeriod = 100 * 1000;
// //     SFE_UBLOX_GNSS neom9n;
// //     Comms::Packet latLongPacket = {.id = 6};
// //     Comms::Packet auxPacket = {.id = 7};



// //     //
// //     void initGPS() {
// //         neom9n.begin();
// //     };

//     uint32_t sampleGPS() {
//         Get Longtitude
//         neom9n.checkUblox();
//         float latitude = neom9n.getLatitude();
//         float longitude = neom9n.getLongitude(); 
//         float altitude = neom9n.getAltitude();
//         float speed = neom9n.getSpeedAccEst();

//         Serial.print(latitude);
//         Serial.print(",");
//         Serial.print(longitude);
//         Serial.print(",");
//         Serial.print(altitude);
//         Serial.print(",");
//         Serial.println(speed);

//         latLongPacket.len = 0;
//         Comms::packetAddFloat(&latLongPacket, latitude);
//         Comms::packetAddFloat(&latLongPacket, longitude);
//         Comms::packetAddFloat(&latLongPacket, altitude);
//         Comms::packetAddFloat(&latLongPacket, speed);
//         Comms::emitPacket(&latLongPacket);

//         return gpsUpdatePeriod;
//     }

    
// //     uint32_t auxSample() {
// //         //Get Altitude
// //         // altitude = HAL::neom9n.getAltitude();

// //         // //Get SIV
// //         // SIV = HAL::neom9n.getSIV();

// //         // //Get Angle


// //         // //Get Speed

// //         // auxPacket.len = 0;
// //         // Comms::packetAddFloat(&auxPacket, altitude);
// //         // Comms::packetAddFloat(&auxPacket, SIV);
// //         // Comms::emitPacket(&auxPacket);

// //         return gpsUpdatePeriod;
// //     }
// }