/*
INA219.h - Header file for the Zero-Drift, Bi-directional Current/Power Monitor Arduino Library.

Version: 1.0.0
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INA219_h
#define INA219_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <INA.h>

#define INA219_ADDRESS              (0x40)

#define INA219_CMD_READ             (0x01)

#define INA219_REG_CONFIG           (0x00)
#define INA219_REG_SHUNTVOLTAGE     (0x01)
#define INA219_REG_BUSVOLTAGE       (0x02)
#define INA219_REG_POWER            (0x03)
#define INA219_REG_CURRENT          (0x04)
#define INA219_REG_CALIBRATION      (0x05)


class INA219 : public INA
{

  protected:
TwoWire *_localWire;
    public:

	bool begin(TwoWire *localWire, uint8_t address = INA219_ADDRESS);
	bool configure(ina219_range_t range = INA219_RANGE_32V, ina219_gain_t gain = INA219_GAIN_320MV, ina219_busRes_t busRes = INA219_BUS_RES_12BIT, ina219_shuntRes_t shuntRes = INA219_SHUNT_RES_12BIT_1S, ina219_mode_t mode = INA219_MODE_SHUNT_BUS_CONT);
  // don't use the following. just exists for interface compatibility.
  bool configure(ina226_averages_t avg = INA226_AVERAGES_1, ina226_busConvTime_t busConvTime = INA226_BUS_CONV_TIME_1100US, ina226_shuntConvTime_t shuntConvTime = INA226_SHUNT_CONV_TIME_1100US, ina226_mode_t mode = INA226_MODE_SHUNT_BUS_CONT);
  bool calibrate(float rShuntValue = 0.1, float iMaxExcepted = 2);

	ina219_range_t getRange(void);
	ina219_gain_t getGain(void);
	ina219_busRes_t getBusRes(void);
	ina219_shuntRes_t getShuntRes(void);
	ina219_mode_t getMode(void);

	float readShuntCurrent(void);
	float readShuntVoltage(void);
	float readBusPower(void);
	float readBusVoltage(void);

	float getMaxPossibleCurrent(void);
	float getMaxCurrent(void);
	float getMaxShuntVoltage(void);
	float getMaxPower(void);

    private:

	int8_t inaAddress;
	float currentLSB, powerLSB;
	float vShuntMax, vBusMax, rShunt;

	void writeRegister16(uint8_t reg, uint16_t val);
	int16_t readRegister16(uint8_t reg);
};

#endif
