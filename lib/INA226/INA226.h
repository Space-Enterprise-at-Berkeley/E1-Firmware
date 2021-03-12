/*
INA226.h - Header file for the Bi-directional Current/Power Monitor Arduino Library.

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

#ifndef INA226_h
#define INA226_h

#include <Arduino.h>
#include <INA.h>

#define INA226_ADDRESS              (0x40)

#define INA226_REG_CONFIG           (0x00)
#define INA226_REG_SHUNTVOLTAGE     (0x01)
#define INA226_REG_BUSVOLTAGE       (0x02)
#define INA226_REG_POWER            (0x03)
#define INA226_REG_CURRENT          (0x04)
#define INA226_REG_CALIBRATION      (0x05)
#define INA226_REG_MASKENABLE       (0x06)
#define INA226_REG_ALERTLIMIT       (0x07)

#define INA226_BIT_SOL              (0x8000)
#define INA226_BIT_SUL              (0x4000)
#define INA226_BIT_BOL              (0x2000)
#define INA226_BIT_BUL              (0x1000)
#define INA226_BIT_POL              (0x0800)
#define INA226_BIT_CNVR             (0x0400)
#define INA226_BIT_AFF              (0x0010)
#define INA226_BIT_CVRF             (0x0008)
#define INA226_BIT_OVF              (0x0004)
#define INA226_BIT_APOL             (0x0002)
#define INA226_BIT_LEN              (0x0001)

class INA226 : public INA
{
    protected:
  TwoWire *_wire;

    public:

	bool begin(TwoWire *localWire, uint8_t address = INA226_ADDRESS);
	bool configure(ina226_averages_t avg = INA226_AVERAGES_1, ina226_busConvTime_t busConvTime = INA226_BUS_CONV_TIME_1100US, ina226_shuntConvTime_t shuntConvTime = INA226_SHUNT_CONV_TIME_1100US, ina226_mode_t mode = INA226_MODE_SHUNT_BUS_CONT);
  // do not use. Only exists for interface compatibility.
  bool configure(ina219_range_t range = INA219_RANGE_32V, ina219_gain_t gain = INA219_GAIN_320MV, ina219_busRes_t busRes = INA219_BUS_RES_12BIT, ina219_shuntRes_t shuntRes = INA219_SHUNT_RES_12BIT_1S, ina219_mode_t mode = INA219_MODE_SHUNT_BUS_CONT);
  bool calibrate(float rShuntValue = 0.1, float iMaxExcepted = 2);

	ina226_averages_t getAverages(void);
	ina226_busConvTime_t getBusConversionTime(void);
	ina226_shuntConvTime_t getShuntConversionTime(void);
	ina226_mode_t getMode(void);

	void enableShuntOverLimitAlert(void);
	void enableShuntUnderLimitAlert(void);
	void enableBusOvertLimitAlert(void);
	void enableBusUnderLimitAlert(void);
	void enableOverPowerLimitAlert(void);
	void enableConversionReadyAlert(void);

	void setBusVoltageLimit(float voltage);
	void setShuntVoltageLimit(float voltage);
	void setPowerLimit(float watts);

	void setAlertInvertedPolarity(bool inverted);
	void setAlertLatch(bool latch);

	bool isMathOverflow(void);
	bool isAlert(void);

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

	void setMaskEnable(uint16_t mask);
	uint16_t getMaskEnable(void);

	void writeRegister16(uint8_t reg, uint16_t val);
	int16_t readRegister16(uint8_t reg);
};

#endif
