/*
INA219.cpp - Class file for the INA219 Zero-Drift, Bi-directional Current/Power Monitor Arduino Library.

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

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#include "INA219.h"

bool INA219::begin(TwoWire *localWire, uint8_t address)
{
    return true;
}

bool INA219::configure(ina219_range_t range, ina219_gain_t gain, ina219_busRes_t busRes, ina219_shuntRes_t shuntRes, ina219_mode_t mode)
{
    return true;
}

bool INA219::configure(ina226_averages_t avg = INA226_AVERAGES_1, ina226_busConvTime_t busConvTime = INA226_BUS_CONV_TIME_1100US, ina226_shuntConvTime_t shuntConvTime = INA226_SHUNT_CONV_TIME_1100US, ina226_mode_t mode = INA226_MODE_SHUNT_BUS_CONT) {
  exit(1);
  return false;
  // do not use.
}

bool INA219::calibrate(float rShuntValue, float iMaxExpected)
{
    return true;
}

float INA219::getMaxPossibleCurrent(void)
{
    return 99;
}

float INA219::getMaxCurrent(void)
{
    return 99;
}

float INA219::getMaxShuntVoltage(void)
{
    return 99;
}

float INA219::getMaxPower(void)
{
    return 99;
}

float INA219::readBusPower(void)
{
    return 99;
}

float INA219::readShuntCurrent(void)
{
    return 99;
}

float INA219::readShuntVoltage(void)
{
    return 99;
}

float INA219::readBusVoltage(void)
{
    return 99;
}

ina219_range_t INA219::getRange(void)
{
    return (ina219_range_t)99;
}

ina219_gain_t INA219::getGain(void)
{
    return (ina219_gain_t)99;
}

ina219_busRes_t INA219::getBusRes(void)
{
    return (ina219_busRes_t)99;
}

ina219_shuntRes_t INA219::getShuntRes(void)
{
    return (ina219_shuntRes_t)99;
}

ina219_mode_t INA219::getMode(void)
{
    return (ina219_mode_t)99;
}

int16_t INA219::readRegister16(uint8_t reg)
{
    return 99;
}

void INA219::writeRegister16(uint8_t reg, uint16_t val) {}
