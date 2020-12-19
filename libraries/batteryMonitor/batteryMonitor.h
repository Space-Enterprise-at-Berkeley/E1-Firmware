/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/
#ifndef __BATT_MON__
#define __BATT_MON__

#include <INA226.h> //https://github.com/jarzebski/Arduino-INA226

using namespace std;

namespace batteryMonitor {

  INA226 ina;

  float energyConsumed = 0;
  long last_checked;

  void init() {
    ina.begin();
    ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    ina.calibrate(0.002, 10);
  }

  void readVoltage(float *data) {
    data[0] = ina.readBusVoltage();
    data[1] = -1;
  }

  void readPowerConsumption(float *data) {
    data[0] = ina.readBusPower();
    data[1] = -1;

    energyConsumed += data[0] * (millis() - last_checked) / 1000;
    last_checked = millis();
  }

  void getEnergyConsumption(float *data) {
    readPowerConsumption(data);
    data[0] = energyConsumed;
  }

  void readAllBatteryStats(float *data) {
    readPowerConsumption(data);
    data[1] = data[0];
    data[0] = ina.readBusVoltage();
    data[2] = -1*ina.readShuntCurrent(); //wiring is flipped on pcb
    data[3] = -1;
  }

}


#endif /* end of include guard: BATT_MON */
