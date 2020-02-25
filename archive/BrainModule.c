#include <SoftwareSerial.h>

// This ID Code should be set to a value specific to each module
#define ID_CODE 1

// Define pin parameters for each module
#define SSRX 5
#define SSTX 6
#define SSDE 12

// Define MACROs to be used throughout the codes
// Message structure = ID_CODE/VALUE/CONNECTION
#define RECEIVE 1
#define TRANSMIT 0
#define CONFIRM 200
#define FAULT_CODE 111
//
#define COMMAND_1 1
#define COMMAND_2 2
#define COMMAND_3 3
#define COMMAND_4 4
#define COMMAND_5 5
#define COMMAND_6 6
#define COMMAND_7 7
#define COMMAND_8 8
#define COMMAND_9 9
#define COMMAND_10 10

int loxMotorPos;
int propMotorPos;

int loxPressure;
int propPressure;

int nLoxPressure;
int nPropPressure;

void recieve_message(Stream &port);
void send_message(char message, Stream &port);

void command1(int value);
void command2(int value);
void command3(int value);
void command4(int value);
void command5(int value);
void command6(int value);
void command7(int value);
void command8(int value);
void command9(int value);
void command10(int value);

// Define abstractions
SoftwareSerial rs485serial(SSRX, SSTX); //rx, tx

void setup() {
	// initialize the serial ports
	rs485serial.begin(4800);
	pinMode(SSDE, OUTPUT);
	digitalWrite(SSDE, LOW);
	Serial.begin(9600);

	Serial.println("Serial connections initialized.");

	//check motor connections

	Serial.println("Motors initialized.");

	//check sensor connections

	Serial.println("Sensors initialized.")

	// User friendly status message for debugging
	Serial.println("setup done");
}

void loop() {
	recieve_message(rs485serial, SSDE);
	recieve_message(Serial, 255);


}


void recieve_message(Stream &port, int controlPin) {
	if (port.available()) {
		int identifier = port.read();
		//Check if message is intended for Brain
		if (identifier == ID_CODE) {
			int value = port.read();
			int connection = port.read();

			switch (connection) {

			case COMMAND_1:
				command1(value);
				send_message(port, CONFIRM, controlPin);
				break;

			case COMMAND_2:
				command2(value);
				send_message(port, CONFIRM, controlPin);
				break;

			case COMMAND_3:
				command3(value);
				send_message(port, CONFIRM, controlPin);
				break;

			case COMMAND_4:
				command4(value);
				send_message(port, CONFIRM, controlPin);
				break;

			case COMMAND_5:
				command5(value);
				send_message(port, CONFIRM, controlPin);
				break;

			case COMMAND_6:
				command6(value);
				send_message(port, CONFIRM, controlPin);
				break;

			case COMMAND_7:
				command7(value);
				send_message(port, CONFIRM, controlPin);
				break;

			case COMMAND_8:
				command8(value);
				send_message(port, CONFIRM, controlPin);
				break;

			case COMMAND_9:
				command9(value);
				send_message(port, CONFIRM, controlPin);
				break;

			case COMMAND_10:
				command10(value);
				send_message(port, CONFIRM, controlPin);
				break;

			default:
				send_message(port, FAULT_CODE, controlPin);
				break;
			}
		}
	}
}

void send_message(Stream &port, char message, int controlPin) {

	// Allow for flow control to be bypassed.
	if (controlPin == 255) {
		port.write(message);
	}
	// Implement flow control for serial connections which require it.
	else {
		digitalWrite(controlPin, TRANSMIT);
		port.write(message);
		digitalWrite(controlPin, RECEIVE);
	}
}

// ~~~~ Primary commands ~~~~
// Return reading from N2 sensor for lox line
void command1(int value) {
	
}
// Return reading from sensor input 2
void command2(int value) {
	int value = adjustValue(Sensor2);
	char message = itoa(Sensor2.value);
	send_message(rs485serial, message, SSDE);
}
// Return reading from sensor input 3
void command3(int value) {
	int value = adjustValue(Sensor3);
	char message = itoa(Sensor3.value);
	send_message(rs485serial, message, SSDE);
}
// Return reading from sensor input 4
void command4(int value) {
	int value = adjustValue(Sensor4);
	char message = itoa(Sensor4.value);
	send_message(rs485serial, message, SSDE);
}
void command5(int value) {

}
void command6(int value) {

}
void command7(int value) {

}

// ~~~~ Debugging Commands ~~~~
// Enable LED on pin 13 for debugging
void command8(int value) {
	digitalWrite(LED_BUILTIN, HIGH);
}
// Return debugging message on hardware serial
void command9(int value) {
	char message = "Hello World.";
	send_message(Serial, message, 255);
}
// Return debugging message on RS485 Software serial
void command10(int value) {
	char message = "Hello RS485 World.";
	send_message(rs485serial, message, SSDE);
}
