/*
  batteryMonitor.h - A c++ library to interface w/ the ina226 chip and read battery stats.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/
#ifndef __ACT_MON__
#define __ACT_MON__

/*
   HOLD OFF ON THIS.
   I THINK I'LL JUST ADD An INA219 object to each
   ACTUATor.
 */
#include <INA219.h> //https://github.com/jarzebski/Arduino-INA226

using namespace std;

namespace ActuatorMonitor {
  INA219 ** inas;

  float energyConsumed = 0;
  long last_checked;

  void init(TwoWire *localWire, float rShunt, float maxExpectedCurrent) {
    ina.begin(localWire);
    ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    ina.calibrate(rShunt, maxExpectedCurrent);
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
