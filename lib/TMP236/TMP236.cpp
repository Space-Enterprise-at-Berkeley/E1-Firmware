/*
  FDC2214.cpp - A c++ library to interface with the FDC2214 
  capacitance to digital converter
  Created by Justin Yang, Oct 23, 2021.
*/                   

#include "TMP236.h"

TMP236::TMP236(uint8_t analog_pin) { _analog_pin = analog_pin; }

void TMP236::init() {
    pinMode(_analog_pin, INPUT);
}

float TMP236::readTemperature(){
    float temp_gain = 19.5; // mV/C
    float temp_offset = 400; // mV
    
    float temp_voltage = analogReadMilliVolts(_analog_pin);

    float sensor_temp = (temp_voltage-temp_offset)/temp_gain;
    return sensor_temp;
}