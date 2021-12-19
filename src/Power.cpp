#include <Power.h>

namespace Power {

    // TODO: change this to appropriate value
    uint32_t powerUpdatePeriod = 1 * 1000;

    long last_checked[HAL::numSupplies];
    float voltages[HAL::numSupplies];
    float currents[HAL::numSupplies];
    float powers[HAL::numSupplies];
    float energyConsumed[HAL::numSupplies];

    void initPower(){

        // initialize buffers
        for (int i = 0; i < HAL::numSupplies; i++) {
            last_checked[i] = 0;
            voltages[i] = 0;
            currents[i] = 0;
            powers[i] = 0;
            energyConsumed[i] = 0;
        }

    }

    uint32_t powerSample() {
        // Reads all current, voltage, power values for all power supplies

        for (int target = 0; target < HAL::numSupplies; target++){
            voltages[target] = HAL::supplyMonitors[target]->readBusVoltage();
            currents[target] = HAL::supplyMonitors[target]->readShuntCurrent();
            powers[target] = HAL::supplyMonitors[target]->readBusPower();
            energyConsumed[target] += powers[target] * (millis() - last_checked[target]) / 1000;
            last_checked[target] = millis();
            // TODO: remove this janky ass test code 
            Serial.println("Read values from "+String(target));
            Serial.println("Voltage: " + String(voltages[target]) + "V \tCurrent: " + String(currents[target]) + "A \tPower: " + String(powers[target]));
        }

        return powerUpdatePeriod;
    }
}
