#include "ADS8167.h"
#include "ADS8167_reg.h"

#include <string.h>

#include <Arduino.h>
#include <SPI.h>

#include <math.h>
#include "HardwareSerial.h"
// #define LOG_LOCAL_LEVEL     ESP_LOG_VERBOSE
// #define DBG(msg, ...)      Serial.printf("%s: " msg "\n", __FUNCTION__, ##__VA_ARGS__)

// Helpers
#define CMD_ADDRESS_MASK    ((uint16_t)(1 << 11) - 1)

#define N_CHANNELS          8

ADS8167::ADS8167(SPIClass *theSPI, uint8_t cs, uint8_t rdy, uint8_t alrt):
  ADC(rdy)
{
  // do nothing;
}

ADS8167::ADS8167():
  ADC()
{
  // do nothing;
}

bool ADS8167::init(SPIClass *theSPI, uint8_t cs, uint8_t rdy, uint8_t alrt):
{
  // do nothing;
}

bool ADS8167::init() {
  // do nothing;
    return true;
}

/*
 *
 */
void ADS8167::setSDOMode() {
// do nothing;
}

void ADS8167::enableSeqStatus(bool en) {
  // do nothing;
}

void ADS8167::enableParityBit(bool en) {
// do nothing;
}

/*
 * sets all inputs to be distinct. As opposed to differential inputs.
 */
void ADS8167::setAllInputsSeparate(){
  // do nothing;
}

/*
 * for manual mode.
 */
void ADS8167::setChannel(const uint8_t channelno) {
// do nothing;
}

void ADS8167::setManualMode() {
// do nothing;
}

void ADS8167::setAutoSequenceMode() {
// do nothing;
}

void ADS8167::setSequence(const uint8_t length, const uint8_t* channels, const uint8_t repeat, const bool loop) {
  // do nothing;
}

void ADS8167::sequenceStart() {
// do nothing;
}

uint16_t ADS8167::readChannel(uint8_t* channel_out) {
  // do nothing;
  return 99;
}

long ADS8167::readData(uint8_t channel_no) {
  return readChannel();
}

void ADS8167::enableOTFMode() {
    // do nothing;
  }

void ADS8167::waitForDataReady() {
  // do nothing;
}

uint16_t ADS8167::readChannelOTF(const uint8_t otf_next_channel, uint8_t* channel_out) {
    // do nothing;
    return 99;
}

void ADS8167::write_cmd(const adc_cmd_t cmd, const uint16_t address, const uint8_t data) {
    // do nothing;
}
