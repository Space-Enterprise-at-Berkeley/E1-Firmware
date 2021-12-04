#pragma once
#include <MockSerial.h>
#include <stdlib.h>
#include <thread>
#include <chrono>

#define HIGH			    1
#define LOW			        0
#define INPUT			    0
#define OUTPUT			    1
#define INPUT_PULLUP		2
#define INPUT_PULLDOWN		3
#define OUTPUT_OPENDRAIN	4
#define INPUT_DISABLE		5

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
void analogWrite(uint8_t pin, uint8_t val);
void digitalWriteFast(uint8_t pin, uint8_t val);
void delay(uint32_t millis);
uint32_t millis();
uint32_t micros();
void yield();