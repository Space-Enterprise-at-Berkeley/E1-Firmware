/*
INA226.cpp - Class file for the INA226 Bi-directional Current/Power Monitor Arduino Library.

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

#include "INA226.h"

bool INA226::begin(TwoWire *localWire, uint8_t address)
{
    return true;
}

bool INA226::configure(ina226_averages_t avg, ina226_busConvTime_t busConvTime, ina226_shuntConvTime_t shuntConvTime, ina226_mode_t mode)
{
    return true;
}

bool INA226::configure(ina219_range_t range = INA219_RANGE_32V, ina219_gain_t gain = INA219_GAIN_320MV, ina219_busRes_t busRes = INA219_BUS_RES_12BIT, ina219_shuntRes_t shuntRes = INA219_SHUNT_RES_12BIT_1S, ina219_mode_t mode = INA219_MODE_SHUNT_BUS_CONT) {
  exit(1);
  // do not use.
  return false;
}

bool INA226::calibrate(float rShuntValue, float iMaxExpected)
{
    return true;
}

float INA226::getMaxPossibleCurrent(void)
{
    return 99;
}

float INA226::getMaxCurrent(void)
{
  return 99;
}

float INA226::getMaxShuntVoltage(void)
{
    return 99;
}

float INA226::getMaxPower(void)
{
    return 99;
}

float INA226::readBusPower(void)
{
    return 99;
}

float INA226::readShuntCurrent(void)
{
    return 99;
}

float INA226::readShuntVoltage(void)
{
    return 99;
}

float INA226::readBusVoltage(void)
{
    return 99;
}

ina226_averages_t INA226::getAverages(void)
{
    return (ina226_averages_t)0;
}

ina226_busConvTime_t INA226::getBusConversionTime(void)
{
    return (ina226_busConvTime_t)0;
}

ina226_shuntConvTime_t INA226::getShuntConversionTime(void)
{
    return (ina226_shuntConvTime_t)0;
}

ina226_mode_t INA226::getMode(void)
{
    return (ina226_mode_t)0;
}

void INA226::setMaskEnable(uint16_t mask)
{
    ;
}

uint16_t INA226::getMaskEnable(void)
{
    return 99;
}

void INA226::enableShuntOverLimitAlert(void)
{
    ;
}

void INA226::enableShuntUnderLimitAlert(void)
{
    ;
}

void INA226::enableBusOvertLimitAlert(void)
{
  ;
}

void INA226::enableBusUnderLimitAlert(void)
{
    ;
}

void INA226::enableOverPowerLimitAlert(void)
{
;}

void INA226::enableConversionReadyAlert(void)
{
;}

void INA226::setBusVoltageLimit(float voltage)
{
    ;
}

void INA226::setShuntVoltageLimit(float voltage)
{
  ;
}

void INA226::setPowerLimit(float watts)
{
  ;
}

void INA226::setAlertInvertedPolarity(bool inverted)
{
;
}

void INA226::setAlertLatch(bool latch)
{
  ;
}

bool INA226::isMathOverflow(void)
{
    return true;
}

bool INA226::isAlert(void)
{
  return true;
}

int16_t INA226::readRegister16(uint8_t reg)
{
    return 99;
}

void INA226::writeRegister16(uint8_t reg, uint16_t val)
{
  ;
}
