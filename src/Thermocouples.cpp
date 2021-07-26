#include <Thermocouples.h>

namespace Thermocouples
{

  //init function for a TC instance
  //taken from old code
  int Thermocouple::init(Adafruit_MCP9600 *cryo_board, uint8_t * addrs, _themotype * type, TwoWire *theWire) {
     _addrs = addrs;
     _cryo_amp_board = cryo_board;


    if (!_cryo_amp_board.begin(addrs, theWire)) {
         Serial.println("Error initializing cryo board at Addr 0x" + String(addrs, HEX));
         return -1;
       }

    _cryo_amp_board.setADCresolution(MCP9600_ADCRESOLUTION_16);
    _cryo_amp_board.setThermocoupleType(type);
    _cryo_amp_board.setFilterCoefficient(0);
    _cryo_amp_board.enable(true);

     return 0;
   }

//function for reading value for TC amp.
//record the timestamp and the read value; both of which are variables defined in the header file.
  void Thermocouple::run() {
    value = cryo_amp_board.readThermocouple();
    timestamp = micros();
  }

  //creating Thermocouple instances, each corresponding to a specific TC used in the system.
  Thermocouple TC1;
  Thermocouple TC2;
  Thermocouple TC3;

  //array of all Thermocouples
  Thermocouple allThermocouples [] = {TC1, TC2, TC3};

  //function within Thermocouples namespace that intialiazes all Thermocouple instances, as defined above.
  void initializeAll() {
    TC1.init(/*parameters here*/);
    TC2.init(/*parameters here*/);
    TC3.init(/*parameters here*/);

    //adding TC instances to the scheduer
    Scheduler::repeatTask(&TC1, 200 * 1000);
    Scheduler::repeatTask(&TC2, 200 * 1000);
    Scheduler::repeatTask(&TC3, 200 * 1000);

  }
}
