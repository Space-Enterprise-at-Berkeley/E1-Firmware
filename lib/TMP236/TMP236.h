/*
  FDC2214.h - A c++ library to interface with the FDC2214 
  capacitance to digital converter
  Created by Justin Yang, Oct 23, 2021.
*/                   

#ifndef __TMP236__
#define __TMP236__

#include <Arduino.h>

class TMP236 {
public:
    TMP236(uint8_t analog_pin);
    void init();
       
    float readTemperature();
protected:
  uint8_t _analog_pin = 0;
};

#endif