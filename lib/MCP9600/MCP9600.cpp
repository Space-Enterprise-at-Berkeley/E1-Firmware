#include <MCP9600.h>

MCP9600::MCP9600() {}

/**************************************************************************/
/*!
    @brief  Sets up the I2C connection and tests that the sensor was found.
    @param i2c_addr The I2C address of the target device, default is 0x67
    @param theWire Pointer to an I2C device we'll use to communicate
    default is Wire
    @return true if sensor was found, otherwise false.
*/
/**************************************************************************/
void MCP9600::init(uint8_t i2c_addr,
                   TwoWire *theWire,
                   MCP9600_ADCResolution resolution,
                   MCP9600_ThemocoupleType tcType,
                   uint8_t fCoefficient) {
    address = i2c_addr;
    wire = theWire;

    writeReg(MCP9600_DEVICECONFIG, 0x80 | (resolution << 5));
    writeReg(MCP9600_SENSORCONFIG, (tcType << 4) | (fCoefficient & 0x7));
}

/**************************************************************************/
/*!
    @brief  Read temperature at the end of the thermocouple
    @return Floating point temperature in Centigrade
*/
/**************************************************************************/
float MCP9600::readThermocouple(void) {
    wire->beginTransmission(address);
    wire->write(0x00);
    wire->endTransmission();

    wire->requestFrom(address, 2);
    int16_t tmp = (wire->read() << 8 | wire->read());
    return ((float) tmp) * 0.0625;
}

void MCP9600::writeReg(uint8_t reg, uint8_t value) {
    wire->beginTransmission(address);
    wire->write(reg);
    wire->write(value);
    wire->endTransmission();
}
