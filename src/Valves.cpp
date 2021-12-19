#include <Valves.h>

namespace Valves {

    // TODO: change this to appropriate value
    uint32_t valveCheckPeriod = 1 * 1000; 

    bool valve_is_open[HAL::numValves];
    long last_checked[HAL::numValves];
    float currents[HAL::numValves];
    float voltages[HAL::numValves];

    void initValves(){

        // initialize buffers
        for (int i = 0; i < HAL::numValves; i++) {
            valve_is_open[i] = false;
            last_checked[i] = 0;
            currents[i] = 0;
            voltages[i] = 0;
        }

    }

    uint32_t check_currents() {
        for (int target = 0; target < HAL::numValves; target++){
            currents[target] = HAL::valveMonitors[target]->readShuntCurrent();
            voltages[target] = HAL::valveMonitors[target]->readBusVoltage();
            last_checked[target] = millis();
            if (currents[target] > maxValveCurrent) { // overcurrent event detected
                open_valve(target);
            }
            // TODO: remove this janky ass test code 
            Serial.println("Read values from "+String(target));
            Serial.println("Voltage: " + String(voltages[target]) + "V \tCurrent: " + String(currents[target]) + "A");
        
        }

        return valveCheckPeriod;
    }
    
    void open_valve(int valve_num) {
        digitalWriteFast(HAL::valvePins[valve_num], false);
        valve_is_open[valve_num] = true;
    }

    void close_valve(int valve_num) {
        digitalWriteFast(HAL::valvePins[valve_num], true);
        valve_is_open[valve_num] = false;
    }

}