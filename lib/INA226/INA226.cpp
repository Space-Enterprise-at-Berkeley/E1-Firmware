#include <INA226.h>

void INA226::init(TwoWire *localWire,
                  uint8_t address,
                  float shuntR,
                  float currMax,
                  ina226_averages_t avg = INA226_AVERAGES_1,
                  ina226_busConvTime_t busConvTime = INA226_BUS_CONV_TIME_1100US,
                  ina226_shuntConvTime_t shuntConvTime = INA226_SHUNT_CONV_TIME_1100US,
                  ina226_mode_t mode = INA226_MODE_SHUNT_BUS_CONT)
{
    _wire = localWire;
    inaAddress = address;

    // configuration
    uint16_t config = 0;

    config |= (avg << 9 | busConvTime << 6 | shuntConvTime << 3 | mode);

    vBusMax = 36;
    vShuntMax = 0.08192f;

    writeRegister16(INA226_REG_CONFIG, config);

    // calibration
    uint16_t calibrationValue;
    rShunt = shuntR;

    float iMaxPossible, minimumLSB;

    iMaxPossible = vShuntMax / rShunt;

    minimumLSB = currMax / 32767;

    currentLSB = (uint16_t)(minimumLSB * 100000000);
    currentLSB /= 100000000;
    currentLSB /= 0.0001;
    currentLSB = ceil(currentLSB);
    currentLSB *= 0.0001;

    powerLSB = currentLSB * 25;

    calibrationValue = (uint16_t)((0.00512) / (currentLSB * rShunt));

    writeRegister16(INA226_REG_CALIBRATION, calibrationValue);
}

float INA226::getMaxPossibleCurrent(void)
{
    return (vShuntMax / rShunt);
}

float INA226::getMaxCurrent(void)
{
    float maxCurrent = (currentLSB * 32767);
    float maxPossible = getMaxPossibleCurrent();

    if (maxCurrent > maxPossible)
    {
        return maxPossible;
    } else
    {
        return maxCurrent;
    }
}

float INA226::getMaxShuntVoltage(void)
{
    float maxVoltage = getMaxCurrent() * rShunt;

    if (maxVoltage >= vShuntMax)
    {
        return vShuntMax;
    } else
    {
        return maxVoltage;
    }
}

float INA226::getMaxPower(void)
{
    return (getMaxCurrent() * vBusMax);
}

float INA226::readBusPower(void)
{
    return (readRegister16(INA226_REG_POWER) * powerLSB);
}

float INA226::readShuntCurrent(void)
{
    return (readRegister16(INA226_REG_CURRENT) * currentLSB);
}

float INA226::readShuntVoltage(void)
{
    float voltage;

    voltage = readRegister16(INA226_REG_SHUNTVOLTAGE);

    return (voltage * 0.0000025);
}

float INA226::readBusVoltage(void)
{
    int16_t voltage;

    voltage = readRegister16(INA226_REG_BUSVOLTAGE);

    return (voltage * 0.00125);
}

void INA226::setMaskEnable(uint16_t mask)
{
    writeRegister16(INA226_REG_MASKENABLE, mask);
}

uint16_t INA226::getMaskEnable(void)
{
    return readRegister16(INA226_REG_MASKENABLE);
}

void INA226::enableShuntOverLimitAlert(void)
{
    writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_SOL);
}

void INA226::enableShuntUnderLimitAlert(void)
{
    writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_SUL);
}

void INA226::enableBusOvertLimitAlert(void)
{
    writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_BOL);
}

void INA226::enableBusUnderLimitAlert(void)
{
    writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_BUL);
}

void INA226::enableOverPowerLimitAlert(void)
{
    writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_POL);
}

void INA226::enableConversionReadyAlert(void)
{
    writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_CNVR);
}

void INA226::setBusVoltageLimit(float voltage)
{
    uint16_t value = voltage / 0.00125;
    writeRegister16(INA226_REG_ALERTLIMIT, value);
}

void INA226::setShuntVoltageLimit(float voltage)
{
    uint16_t value = voltage * 25000;
    writeRegister16(INA226_REG_ALERTLIMIT, value);
}

void INA226::setPowerLimit(float watts)
{
    uint16_t value = watts / powerLSB;
    writeRegister16(INA226_REG_ALERTLIMIT, value);
}

void INA226::setAlertInvertedPolarity(bool inverted)
{
    uint16_t temp = getMaskEnable();

    if (inverted)
    {
        temp |= INA226_BIT_APOL;
    } else
    {
        temp &= ~INA226_BIT_APOL;
    }

    setMaskEnable(temp);
}

void INA226::setAlertLatch(bool latch)
{
    uint16_t temp = getMaskEnable();

    if (latch)
    {
        temp |= INA226_BIT_LEN;
    } else
    {
        temp &= ~INA226_BIT_LEN;
    }

    setMaskEnable(temp);
}

bool INA226::isMathOverflow(void)
{
    return ((getMaskEnable() & INA226_BIT_OVF) == INA226_BIT_OVF);
}

bool INA226::isAlert(void)
{
    return ((getMaskEnable() & INA226_BIT_AFF) == INA226_BIT_AFF);
}

int16_t INA226::readRegister16(uint8_t reg)
{
    int16_t value;

    _wire->beginTransmission(inaAddress);
    _wire->write(reg);
    _wire->endTransmission();

    _wire->beginTransmission(inaAddress);
    _wire->requestFrom(inaAddress, 2);
    while(!_wire->available()) {};
    uint8_t vha = _wire->read();
    uint8_t vla = _wire->read();
    _wire->endTransmission();

    value = ((uint16_t) vha) << 8 | (uint16_t) vla;

    return value;
}

void INA226::writeRegister16(uint8_t reg, uint16_t val)
{
    uint8_t vla;
    vla = (uint8_t)val;
    val >>= 8;

    _wire->beginTransmission(inaAddress);
    _wire->write(reg);
    _wire->write((uint8_t)val);
    _wire->write(vla);
    _wire->endTransmission();
}
