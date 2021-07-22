#include <Thermocouples.h>

namespace Thermocouples
{

  //init function for a TC instance
  //taken from old code
  int Thermocouples::init(Adafruit_MCP9600 *cryo_boards, uint8_t * addrs, _themotype * types, TwoWire *theWire, float *latestReads) {
     _addrs = addrs;
     _latestReads = latestReads;
     _cryo_amp_boards = cryo_boards;


    if (!_cryo_amp_board.begin(addrs, theWire)) {
         Serial.println("Error initializing cryo board at Addr 0x" + String(addrs, HEX));
         return -1;
       }

    _cryo_amp_boards.setADCresolution(MCP9600_ADCRESOLUTION_16);
    _cryo_amp_boards.setThermocoupleType(types);
    _cryo_amp_boards.setFilterCoefficient(0);
    _cryo_amp_boards.enable(true);


    Serial.println(i);
    Serial.println(_latestReads);
    _latestReads[i] = -1;
    Serial.println(_latestReads);
    _latestReads[i] = _cryo_amp_boards.readThermocouple();
    Serial.println(_latestReads);


     return 0;
   }


  void Thermocouples::run() {
    value = cryo_amp_board.readThermocouple();
    timestamp = micros();
  }



  //creating Thermocouple instances, each corresponding to a specific TC used in the system.
  Thermocouples TC1;
  Thermocouples TC2;
  Thermocouples TC3;

  //function within Thermocouples namespace that intialiazes all Thermocouple instances, as defined above.
  void initializeAll() {
    TC1.init(/*parameters here*/);
    TC2.init(/*parameters here*/);
    TC3.init(/*parameters here*/);

    //How to add to scheduler?
  }
}
