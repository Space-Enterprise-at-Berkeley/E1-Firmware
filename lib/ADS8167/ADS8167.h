#pragma once

#include <Arduino.h>
#include <SPI.h>

class ADS8167 {
    public:
        ADS8167();

        bool init(SPIClass *theSPI, uint8_t cs, uint8_t rdy, uint8_t alrt);

        void setSDOMode();
        void enableSeqStatus(bool en);
        void enableParityBit(bool en);

        void setAllInputsSeparate();

        void waitForDataReady();

        // Manual mode
        void setManualMode();
        void        setChannel(const uint8_t channelno);
        uint16_t    readChannel(uint8_t* channel_out = NULL);
        long    readData(uint8_t channelno);
        //Auto Sequence mode
        void setAutoSequenceMode();

        // Custom Sequence mode
        void   setSequence(const uint8_t length, const uint8_t* channels, const uint8_t repeat = 1, bool loop = false);
        void        sequenceStart();

        // On-The-Fly Mode, Crappy 0xFFFF problems..
        void        enableOTFMode();
        uint16_t    readChannelOTF(const uint8_t otf_channel_next, uint8_t* channel_out = NULL);

    protected:
        typedef enum {
            ADCCMD_NOP         = 0b00000,
            ADCCMD_WR_REG      = 0b00001,
            ADCCMD_RD_REG      = 0b00010,
            ADCCMD_SET_BITS    = 0b00011,
            ADCCMD_CLR_BITS    = 0b00100,
            ADCCMD_ONTHEFLY    = 0b10000
        } adc_cmd_t;

        void write_cmd(const adc_cmd_t cmd, const uint16_t address, const uint8_t data);

        uint8_t _cs_pin;
        uint8_t _alrt_pin;
        uint8_t _rdy_pin;
        SPIClass *_theSPI;

        uint8_t buffer[6];
};
