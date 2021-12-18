#include <Power.h>

namespace Power {

    // TODO: change this to appropriate value
    uint32_t ptUpdatePeriod = 1 * 1000;

    long last_checked[numSupplies];
    float voltages[numSupplies];
    float currents[numSupplies];
    float powers[numSupplies];
    float _energyConsumed[numSupplies];
    INA219* _supplyMonitors[numSupplies] = {&(HAL::ina1), &(HAL::ina2), &(HAL::ina3)};

    void initPower(){

        // initialize buffers
        for (int i = 0; i < numSupplies; i++) {
            last_checked[i] = 0;
            voltages[i] = 0;
            currents[i] = 0;
            powers[i] = 0;
            _energyConsumed[i] = 0;
        }

    }

    uint32_t powerSample() {
        // Reads all current, voltage, power values for all power supplies

        for (int target = 0; target < numSupplies; target++){
            voltages[target] = _supplyMonitors[target]->readBusVoltage();
            currents[target] = _supplyMonitors[target]->readShuntCurrent();
            powers[target] = _supplyMonitors[target]->readBusPower();
            _energyConsumed[target] += powers[target] * (millis() - last_checked[target]) / 1000;
            last_checked[target] = millis();
            // TODO: remove this janky ass test code 
            Serial.println("Read values from "+String(target));
            Serial.println("Voltage: " + String(voltages[target]) + "V \tCurrent: " + String(currents[target]) + "A \tPower: " + String(powers[target]));
        }

        return powerUpdatePeriod;
    }
}
