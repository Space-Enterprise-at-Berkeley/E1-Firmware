

#ifndef __ADC_H
#define __ADC_H

class ADC {
    public:
      ADC(uint8_t data_rdy_pin):
      _rdy_pin(data_rdy_pin)
      {}

      virtual long readData(uint8_t channel) = 0;
      uint8_t _rdy_pin;
};

#endif
