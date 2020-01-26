// ~~~~~~~~~~~~~~~~~~ Serial Baud Rates – Teensy 4.0 ~~~~~~~~~~~~~~~~~~
#define usbBaud 57600
//Serial 1, for RFD900+ module, appears to be pins 0 - RX1, 1 - TX1.
#define rfBaud 57600
//Serial 2, for RS485 sensor interface appears to be pins 7 - RX2, 8 - TX2.
#define rs485Baud 57600

// ~~~~~~~~~~~~~~~~~~ RS485 Config ~~~~~~~~~~~~~~~~~~
#define rsControl 3

// ~~~~~~~~~~~~~~~~~~ Sensor Configs ~~~~~~~~~~~~~~~~~~
#define maxSensorID 10
int sensorNum = 0;

// ~~~~~~~~~~~~~~~~~~ Sensor Mappings ~~~~~~~~~~~~~~~~~~

void setup() {
  // Setup serials
  Serial.begin(usbBaud);
  Serial1.begin(rfBaud);
  Serial2.begin(rs485Baud);
  Serial.println("Serial setup.");

  // Allow control of RS485 pins
  pinMode(rsControl, OUTPUT);
  digitalWrite(rsControl, LOW);
}

void loop() {
  // First listen for commands coming in over the RF communications.
  while (Serial1.available() > 0) {
    // TODO: Parse and handle commands
    // Will likely be of (ID:data)
  }
  if (sensorNum >= 0 && sensorNum <= maxSensorID) {
    // Send sensor data
    rsTransmit("(" + sensorNum + ")");
    if (sensorNum == maxSensorID) {
      // Reset the sensorNum
      sensorNum = 0;
    }
  } else {
    sensorNum = 0;
  }
}

// Transmit MESSAGE over rs485.
void rsTransmit(String message) {
  // Set pin to high to have control over transmit.
  digitalWrite(rsControl, HIGH);
  Serial2.print(message);
  // Relinquish control.
  digitalWrite(rsControl, LOW);
}
