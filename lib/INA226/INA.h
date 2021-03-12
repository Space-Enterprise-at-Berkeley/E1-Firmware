

#ifndef __INA_H
#define __INA_H


typedef enum
{
    INA219_RANGE_16V            = 0b0,
    INA219_RANGE_32V            = 0b1
} ina219_range_t;

typedef enum
{
    INA219_GAIN_40MV            = 0b00,
    INA219_GAIN_80MV            = 0b01,
    INA219_GAIN_160MV           = 0b10,
    INA219_GAIN_320MV           = 0b11
} ina219_gain_t;

typedef enum
{
    INA219_BUS_RES_9BIT         = 0b0000,
    INA219_BUS_RES_10BIT        = 0b0001,
    INA219_BUS_RES_11BIT        = 0b0010,
    INA219_BUS_RES_12BIT        = 0b0011
} ina219_busRes_t;

typedef enum
{
    INA219_SHUNT_RES_9BIT_1S    = 0b0000,
    INA219_SHUNT_RES_10BIT_1S   = 0b0001,
    INA219_SHUNT_RES_11BIT_1S   = 0b0010,
    INA219_SHUNT_RES_12BIT_1S   = 0b0011,
    INA219_SHUNT_RES_12BIT_2S   = 0b1001,
    INA219_SHUNT_RES_12BIT_4S   = 0b1010,
    INA219_SHUNT_RES_12BIT_8S   = 0b1011,
    INA219_SHUNT_RES_12BIT_16S  = 0b1100,
    INA219_SHUNT_RES_12BIT_32S  = 0b1101,
    INA219_SHUNT_RES_12BIT_64S  = 0b1110,
    INA219_SHUNT_RES_12BIT_128S = 0b1111
} ina219_shuntRes_t;

typedef enum
{
    INA219_MODE_POWER_DOWN      = 0b000,
    INA219_MODE_SHUNT_TRIG      = 0b001,
    INA219_MODE_BUS_TRIG        = 0b010,
    INA219_MODE_SHUNT_BUS_TRIG  = 0b011,
    INA219_MODE_ADC_OFF         = 0b100,
    INA219_MODE_SHUNT_CONT      = 0b101,
    INA219_MODE_BUS_CONT        = 0b110,
    INA219_MODE_SHUNT_BUS_CONT  = 0b111,
} ina219_mode_t;


typedef enum
{
    INA226_AVERAGES_1             = 0b000,
    INA226_AVERAGES_4             = 0b001,
    INA226_AVERAGES_16            = 0b010,
    INA226_AVERAGES_64            = 0b011,
    INA226_AVERAGES_128           = 0b100,
    INA226_AVERAGES_256           = 0b101,
    INA226_AVERAGES_512           = 0b110,
    INA226_AVERAGES_1024          = 0b111
} ina226_averages_t;

typedef enum
{
    INA226_BUS_CONV_TIME_140US    = 0b000,
    INA226_BUS_CONV_TIME_204US    = 0b001,
    INA226_BUS_CONV_TIME_332US    = 0b010,
    INA226_BUS_CONV_TIME_588US    = 0b011,
    INA226_BUS_CONV_TIME_1100US   = 0b100,
    INA226_BUS_CONV_TIME_2116US   = 0b101,
    INA226_BUS_CONV_TIME_4156US   = 0b110,
    INA226_BUS_CONV_TIME_8244US   = 0b111
} ina226_busConvTime_t;

typedef enum
{
    INA226_SHUNT_CONV_TIME_140US   = 0b000,
    INA226_SHUNT_CONV_TIME_204US   = 0b001,
    INA226_SHUNT_CONV_TIME_332US   = 0b010,
    INA226_SHUNT_CONV_TIME_588US   = 0b011,
    INA226_SHUNT_CONV_TIME_1100US  = 0b100,
    INA226_SHUNT_CONV_TIME_2116US  = 0b101,
    INA226_SHUNT_CONV_TIME_4156US  = 0b110,
    INA226_SHUNT_CONV_TIME_8244US  = 0b111
} ina226_shuntConvTime_t;

typedef enum
{
    INA226_MODE_POWER_DOWN      = 0b000,
    INA226_MODE_SHUNT_TRIG      = 0b001,
    INA226_MODE_BUS_TRIG        = 0b010,
    INA226_MODE_SHUNT_BUS_TRIG  = 0b011,
    INA226_MODE_ADC_OFF         = 0b100,
    INA226_MODE_SHUNT_CONT      = 0b101,
    INA226_MODE_BUS_CONT        = 0b110,
    INA226_MODE_SHUNT_BUS_CONT  = 0b111,
} ina226_mode_t;


class INA {
    public:


      INA() {}
      virtual bool begin(TwoWire *localWire, uint8_t address = 0) = 0;
    	virtual bool configure(ina226_averages_t avg = INA226_AVERAGES_1, ina226_busConvTime_t busConvTime = INA226_BUS_CONV_TIME_1100US, ina226_shuntConvTime_t shuntConvTime = INA226_SHUNT_CONV_TIME_1100US, ina226_mode_t mode = INA226_MODE_SHUNT_BUS_CONT) = 0;

      //virtual bool configure(uint8_t avg, uint8_t busConvTime, uint8_t shuntConvTime, uint8_t mode) = 0;
      virtual bool calibrate(float rShuntValue, float iMaxExcepted) = 0;

      virtual float readShuntCurrent(void) = 0;
      virtual	float readShuntVoltage(void) = 0;
      virtual float readBusPower(void) = 0;
      virtual float readBusVoltage(void) = 0;
};

#endif
