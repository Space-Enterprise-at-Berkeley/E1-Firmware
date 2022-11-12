#include <Common.h>
#include <Comms.h>
#include "Ducers.h"
#include "Actuators.h"
#include "CapFill.h"
#include "Valves.h"
#include "HAL.h"
#include "Thermocouples.h"
// #include "OCHandler.h"

// #include "BlackBox.h"
#include "RadioBlackBox.h"
#include "Barometer.h"
#include "IMU.h"
#include "GPS.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "Apogee.h"

// 0: Ground, 1: Flight
uint8_t vehicleState = 0; 

Task taskTable[] = {
    {Actuators::stopPressFlowRBV, 0, false},
    {Valves::toggleLoxGemValveTask, 0, false},
    {Valves::toggleFuelGemValveTask, 0, false},

    {Valves::autoventLoxGemValveTask, 0},
    {Valves::autoventFuelGemValveTask, 0},

    // ducers
    {Ducers::ptSample, 0},

    // thermocouples
    {Thermocouples::tc0Sample, 0},
    {Thermocouples::tc1Sample, 0},
    {Thermocouples::tc2Sample, 0},
    {Thermocouples::tc3Sample, 0},

    // valves
    {Valves::loxGemValveSample, 0},
    {Valves::fuelGemValveSample, 0},

    // breakwires
    {Valves::breakWire1Sample, 0},
    {Valves::breakWire2Sample, 0},

    // actuator
    {Actuators::pressFlowRBVSample, 0},

    {IMU::imuSample, 0},

    //GPS
    {GPS::latLongSample, 0},

    // Barometer
    {Barometer::sampleAltPressTemp, 0},

    // Cap fill
    {CapFill::sampleCapFill, 0},

    // Apogee
    {Apogee::checkForApogee, 0},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

// Flight/Launch mode enable
uint8_t setVehicleMode(Comms::Packet statePacket, uint8_t ip){                    
    vehicleState = Comms::packetGetUint8(&statePacket, 0);
    // Send confirmation to ground station
    Comms::Packet tmp = {.id = 29};
    Comms::packetAddUint8(&tmp, vehicleState);
    Comms::emitPacket(&tmp);

    // Setup for apogee
    if (vehicleState) { 
        Barometer::zeroAltitude();
        Apogee::start();   
        RadioBlackBox::beginWrite();
    } 

    return vehicleState;
}


int main() {
    // hardware setup
    Serial.begin(115200);

    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif
    HAL::initHAL();
    DEBUG("1\n");
    Comms::initComms();
    DEBUG("2\n");
    Ducers::initDucers();
    DEBUG("3\n");
    Actuators::initActuators(&taskTable[0]);
    DEBUG("4\n");
    Valves::initValves(&taskTable[1], &taskTable[2]);
    DEBUG("5\n");
    GPS::initGPS();
    DEBUG("6\n");
    CapFill::initCapFill();
    DEBUG("7\n");
    Barometer::zeroAltitude();
    DEBUG("8\n");
    Comms::registerCallback(29, setVehicleMode);
    DEBUG("69\n");
    RadioBlackBox::init();

    while(1) {
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
            uint32_t ticks = micros(); // current time in microseconds
            if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
                DEBUG("Task ID: ");
                DEBUG(i);                                                                
                DEBUG("\n");
                DEBUG_FLUSH();
                taskTable[i].nexttime = ticks + taskTable[i].taskCall();
            }
        }
        Comms::processWaitingPackets();
    }
    return 0;
}
