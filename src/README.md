# Main Flight Code

All the different versions of the flight code are defined in this directory with appropriate
environments defined in the `platformio.ini` file. Each different directory has a `.cpp` file
along with a `config.h` file. The config file defines all the sensors, actuators, and commands used by this version of
the code, which is associated with a specific version of the hardware. Since the high level architecture
is all very similar, I will explain a single example here: `E1_coldflow_v2`.

## config.h

Most sensors and input devices are defined in a similar format with a `const uint8_t` defining the number of instances of that sensor/input device followed by a set of arrays defining any associated configuration variables for each instance of the sensor. The following is an example for the thermocouples:
```
// Cryo Thermocouple
const uint8_t numCryoTherms = 4;
uint8_t cryoThermAddrs[numCryoTherms] = {0x60, 0x61, 0x62, 0x63};
_themotype cryoTypes[numCryoTherms] = {MCP9600_TYPE_K, MCP9600_TYPE_K, MCP9600_TYPE_K, MCP9600_TYPE_K};
Adafruit_MCP9600 _cryo_boards[numCryoTherms];
float cryoReadsBackingStore[numCryoTherms];
```

`cryoThermAddrs` defines the I2C addresses for each of the sensors, `cryoTypes` defines a config value for each thermocouple (the `_themotype` data type is defined in the thermocouple library we're using), `_cryo_boards` is the array of instance variables for the thermocouple objects themselves (this will get passed to the thermocouple library to use locally), `cryoReadsBackingStore` is a float array that holds the most recent reads from the thermocouples.

After defining all the sensors and actuators that this code uses, we define the commands that the code listens for. Most of the commands are defined within libraries and the ids are specified in the config file, however, we may define some commands directly in this file. In the following segment we create an array that has references to all the commands we've defined. See `../lib/communications/README.md` for more details about the commands.
```
Command *backingStore[numCommands] = {...};
CommandArray commands(numCommands, backingStore);
```

Within the `setup` function, we instantiate all the objects and call appropriate setup functions for each object.

Following that, we define all the sensors.

## E1_coldflow_v21.cpp


The `setup` function initializes all of our libraries 
