// ~~~~~~~~~~~~~~~~~~ Serial Baud Rates – Teensy 4.0 ~~~~~~~~~~~~~~~~~~
// Serial: USB debug, Serial1: RFD900+, Serial2: RS485
#define usbBaud 57600
//Serial 1, for RFD900+ module, appears to be pins 0 - RX1, 1 - TX1.
#define rfBaud 57600
//Serial 2, for RS485, appears to be pins 7 - RX2, 8 - TX2.
#define rs485Baud 57600

// ~~~~~~~~~~~~~~~~~~ RS485 Config ~~~~~~~~~~~~~~~~~~
// Pin number for receive and transmit modes.
#define rsControl 3
// Milliseconds to wait for non-malformed sensor packet before moving on to the next one.
#define timeoutMillis 500

// ~~~~~~~~~~~~~~~~~~ Sensor Configs ~~~~~~~~~~~~~~~~~~
#define maxSensorID 10
int sensorNum = 0;

// ~~~~~~~~~~~~~~~~~~ Packet Config ~~~~~~~~~~~~~~~~~~
#define startChar '('
#define endChar ')'

void setup() {
  // Setup serials
  Serial.begin(usbBaud);
  Serial1.begin(rfBaud);
  Serial2.begin(rs485Baud);
  Serial.println("Serial setup complete.");

  // Allow control of RS485 pins
  pinMode(rsControl, OUTPUT);
  digitalWrite(rsControl, LOW);
}

void loop() {
  // First listen for commands coming in over the RF communications.
  while (Serial1.available() > 0) {
    // TODO: Parse and handle commands
    // Will likely be of (ID:data)
    // Will need to add something to wait for packet end or timeout.
  }
  if (not (sensorNum >= 0 && sensorNum <= maxSensorID)) {
    // Just in case.
    sensorNum = 0;
  }
  rsTransmit("(" + idToString(sensorNum) + ")");

  // Read a response.
  String packet = parseRsPacket(sensorNum, millis());

  // Make sure we do not go over maxSensorID.
  if (sensorNum == maxSensorID) {
    // Reset the sensorNum
    sensorNum = 0;
  } else {
    sensorNum++;
  }
}

// Transmit MESSAGE over RS485.
void rsTransmit(String message) {
  // Set pin to high to have control over transmit.
  digitalWrite(rsControl, HIGH);
  Serial2.print(message);
  // Relinquish control.
  digitalWrite(rsControl, LOW);
}

// Parse a packet from RS485. 
// Verify received id is ID, and timeout if millis() - STARTPARSEMILLIS >= timeoutMillis.
String parseRsPacket(int id, long startParseMillis) {
  String packet = "";
  int receivedID = 0;
  bool started = false;
  while(millis() - startParseMillis <= timeoutMillis) {
    if (Serial2.available() > 0)
    {
      char inChar = Serial2.read();
      if (inChar == startChar)
      {
        started = true;
      }
      else if (inChar == endChar)
      {
        return packet;
      }
      if (started)
      {
        packet += inChar;
      }
    }
  }
  return "";
}
