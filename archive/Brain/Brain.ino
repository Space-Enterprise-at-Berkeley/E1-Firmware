/*
  Brain.ino - This is the brain of SEB rockets.
  Created by Shao-Qian Mah, Jan 25, 2020.
*/

// ~~~~~~~~~~~~~~~~~~ Serial Config - Teensy 4.0 ~~~~~~~~~~~~~~~~~~
#define USBSERIAL Serial
#define INTERNALSERIAL Serial1
#define RFSERIAL Serial2
#define USBBAUD 57600
//Serial 1, for RFD900+ module, appears to be pins 0 - TX1, 1 - RX1.
#define RFBAUD 57600
//Serial 2, for RS485, appears to be pins 7 - TX2, 8 - RX2.
#define RS485BAUD 57600

// ~~~~~~~~~~~~~~~~~~ RS485 Config ~~~~~~~~~~~~~~~~~~
// Pin number for receive and transmit modes.
#define RSCONTROL 3
// Milliseconds to wait for non-malformed sensor packet before moving on to the next one.
#define TIMEOUTMILLIS 500
// Max number of chars in a message, including '(' and ')' chars.
#define MAXCHARS 100
// Max number of times to retry querying a sensor before giving up.
#define MAXRETRIES 3

// ~~~~~~~~~~~~~~~~~~ Sensor Config ~~~~~~~~~~~~~~~~~~
// CHANGE THIS VALUE!!!
#define MAXSENSORID 10
int sensorNum = 1;

// ~~~~~~~~~~~~~~~~~~ Packet Config ~~~~~~~~~~~~~~~~~~
#define STARTCHAR '('
#define ENDCHAR ')'

void setup() {
  // Setup serials
  USBSERIAL.begin(USBBAUD);
  USBSERIAL.println("Set up USB serial.");
  RFSERIAL.begin(RFBAUD);
  USBSERIAL.println("Set up RF serial.");
  INTERNALSERIAL.begin(RS485BAUD);
  USBSERIAL.println("Set up internal serial.");
  USBSERIAL.println("Serial setup complete.");

  // Allow control of RS485 pins.
  pinMode(RSCONTROL, OUTPUT);
  digitalWrite(RSCONTROL, LOW);
}

void loop() {
  // First listen for commands coming in over the RF communications.
  while (RFSERIAL.available() > 0) {
    // TODO: Parse and handle commands
    // Will likely be of (IDdata)
    // Will need to add something to wait for packet end or timeout.
  }
  if (not (sensorNum >= 1 && sensorNum <= MAXSENSORID)) {
    // Just in case. HAS to be 1 or higher.
    sensorNum = 1;
  }

  // Try sending a sensor request packet and read a response.
  int tries = 0;
  String packet = "";
  String query = "(" + idToString(sensorNum) + ")";

  while (packet.length() == 0 && query.length() > 2 && tries <= MAXRETRIES) {
    internalTransmit(&query);
    packet = readPacket(sensorNum, millis(), MAXCHARS, TIMEOUTMILLIS);
    tries++;
  }

  // Reflect the packet to the RF module if a valid one was received.
  if (packet.length() != 0) {
    RFSERIAL.print(packet);
    // SD PRINT HERE
  }

  // Make sure we do not go over MAXSENSORID.
  if (sensorNum == MAXSENSORID) {
    // Reset the sensorNum
    sensorNum = 1;
  } else {
    sensorNum++;
  }
}
