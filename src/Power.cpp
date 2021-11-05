#include <Power.h>

namespace Power
{   

    PowerTask::PowerTask(Power* power, int target, void (Power::*func)(uint32_t, int)){
        _power = power;
        _target = target;
        _func = func;
    }
    void PowerTask::run(uint32_t exec_time) {
        (_power->*_func)(exec_time, _target);
    }

    // values taken from old code, hotfire/main 68c4e9e
    uint8_t numSupplies = 3;
    uint8_t powSupMonAddrs[] = {0x41, 0x42, 0x43};
    float rShunt = 0.010;
    float maxExpectedCurrent = 5;

    // helper function to instantiate power supply monitors
    Power* initPower(TwoWire *localwire){
        localwire->begin();
        INA219 ** powerSupplyMonitors = (INA219 **)malloc(sizeof(INA219 *) * numSupplies);
        for (int i = 0; i < numSupplies; i++) {
            powerSupplyMonitors[i] = new INA219();
        }
        Power *out = new Power(numSupplies, powerSupplyMonitors, powSupMonAddrs, rShunt, maxExpectedCurrent, localwire);
        return out;
    }

    // 3 power supply monitors: 12V, 8V, Battery
    // uses INA219 - current/voltage/power monitoring

    Power::Power(uint8_t numSupplies, INA219 ** supplyMons, uint8_t * addrs, float rShunt, float maxExpectedCurrent, TwoWire *localWire) {
        this->numSupplies = numSupplies;
        _localWire = localWire;
        _supplyMonitors = supplyMons;

        _energyConsumed = (float *) malloc(numSupplies * sizeof(float));
        last_checked = (long *) malloc((numSupplies) * sizeof(float));
        voltages = (float *) malloc((numSupplies) * sizeof(float));
        currents = (float *) malloc((numSupplies) * sizeof(float));
        powers = (float *) malloc((numSupplies) * sizeof(float));

        for (int i = 0; i < numSupplies; i++) {
            _supplyMonitors[i]->begin(localWire, addrs[i]);
            _supplyMonitors[i]->configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
            _supplyMonitors[i]->calibrate(rShunt, maxExpectedCurrent);
            _energyConsumed[i] = 0;
        }
        //Include code for initializing INA chips
    }

    void Power::readVoltage(uint32_t exec_time, int target) {
        // TODO: record data AND timestamps
        Serial.println("Reading voltage from "+String(target));
        Serial.println(_supplyMonitors[target]->readBusVoltage());
        voltages[target] = _supplyMonitors[target]->readBusVoltage();
        // w h y??
        // voltages[_numSupplies] = -1;
    }

    void Power::readCurrent(uint32_t exec_time, int target) {
        Serial.println("Reading current from "+String(target));
        Serial.println(_supplyMonitors[target]->readShuntCurrent());
        currents[target] = _supplyMonitors[target]->readShuntCurrent();
        // currents[_numSupplies] = -1;
    }

    void Power::readPowerConsumption(uint32_t exec_time, int target) {
        Serial.println("Reading power from "+String(target));
        Serial.println(_supplyMonitors[target]->readBusPower());
        powers[target] = _supplyMonitors[target]->readBusPower();
        _energyConsumed[target] += powers[target] * (millis() - last_checked[target]) / 1000;
        last_checked[target] = millis();
    }

    void Power::readAllBatteryStats(uint32_t exec_time, int target) {
        Serial.println("Reading all from "+String(target));
        this->readVoltage(exec_time, target);
        this->readCurrent(exec_time, target);
    }
}