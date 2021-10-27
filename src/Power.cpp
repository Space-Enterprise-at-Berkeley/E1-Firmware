#include <Power.h>

namespace Power
{   
    // values taken from old code, hotfire/main 68c4e9e
    uint8_t numSupplies = 3;
    uint8_t powSupMonAddrs[] = {0x41, 0x42, 0x43};
    float rShunt = 0.010;
    float maxExpectedCurrent = 5;

    // helper function to instantiate power supply monitors
    Power& initPower(TwoWire *localwire){
        INA219 powerSupplyMonitors[numSupplies];
        INA219 *powSupMonPointers[numSupplies];
        for (int i = 0; i < numSupplies; i++) {
            powSupMonPointers[i] = &powerSupplyMonitors[i];
        }
        Power *out = new Power(numSupplies, powSupMonPointers, powSupMonAddrs, rShunt, maxExpectedCurrent, localwire);
        return *out;
    }

    // 3 power supply monitors: 12V, 8V, Battery
    // uses INA219 - current/voltage/power monitoring

    Power::Power(uint8_t numSupplies, INA219 ** supplyMons, uint8_t * addrs, float rShunt, float maxExpectedCurrent, TwoWire *localWire) {
        _numSupplies = numSupplies;
        _localWire = localWire;
        _addrs = addrs;
        _supplyMonitors = supplyMons;

        _energyConsumed = (float *) malloc(numSupplies * sizeof(float));
        voltages = (float *) malloc((numSupplies) * sizeof(float));
        currents = (float *) malloc((numSupplies) * sizeof(float));
        powers = (float *) malloc((numSupplies) * sizeof(float));

        for (int i = 0; i < _numSupplies; i++) {
            _supplyMonitors[i]->begin(localWire, addrs[i]);
            _supplyMonitors[i]->configure(INA219_RANGE_32V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
            _supplyMonitors[i]->calibrate(rShunt, maxExpectedCurrent);
            debug("started INA219");
            debug("testing");
            _energyConsumed[i] = 0;
            debug("testing2");
        }
        //Include code for initializing INA chips

    }

    void Power::readVoltage(uint32_t exec_time) {
        // TODO: record data AND timestamps
        for (int i = 0; i < _numSupplies; i++){
            voltages[i] = _supplyMonitors[i]->readBusVoltage();
        }
        // w h y??
        // voltages[_numSupplies] = -1;
    }

    void Power::readCurrent(uint32_t exec_time) {
        for (int i = 0; i < _numSupplies; i++){
            currents[i] = _supplyMonitors[i]->readShuntCurrent();
        }
        // currents[_numSupplies] = -1;
    }

    void Power::readPowerConsumption(uint32_t exec_time) {
        for (int i = 0; i < _numSupplies; i++) {
            powers[i] = _supplyMonitors[i]->readBusPower();
            _energyConsumed[i] += powers[i] * (millis() - last_checked) / 1000;
        }
        last_checked = millis();
    }

    void Power::readAllBatteryStats(uint32_t exec_time) {
        this->readVoltage(exec_time);
        this->readCurrent(exec_time);
    }
}
