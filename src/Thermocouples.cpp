#pragma once

#include <Arduino.h>
#include <Comms.h>
#include <Thermocouples.h>

namespace Thermocouples {
    uint32_t tcUpdatePeriod = 10 * 1000;
    Comms::Packet tcPacket;

    float tcTemps[4] = {0.0, 0.0, 0.0, 0.0};
    uint8_t tcAddrs[4] = {0x60, 0x61, 0x62, 0x63};
    Adafruit_MCP9600 TCs[numTCs];

    void initThermocouples(TwoWire *tcBus) {
      //TODO: SET CORRECT ID
      tcPacket.id = 1;

      for (int i = 0; i < numTCs; i++) {
        if (!TCs[i].begin(tcAddrs[i], tcBus)) {
          Serial.println("Error initializing cryo board at Addr 0x" + String(tcAddrs[i], HEX));
          return;
        }

        TCs[i].setADCresolution(MCP9600_ADCRESOLUTION_16);
        TCs[i].setThermocoupleType(MCP9600_TYPE_K);
        TCs[i].setFilterCoefficient(0);
        TCs[i].enable(true);
      }
    }

    uint32_t tcSample() {
        // read from all TCs in sequence
        for (int i = 0; i < numTCs; i++) {
          tcTemps[i] = TCs[i].readThermocouple();
        }
        
        tcPacket.len = 0;
        for (int i = 0; i < numTCs; i++) {
          Comms::packetAddFloat(&tcPacket, tcTemps[i]);
        }

        Comms::emitPacket(&tcPacket);
        // return the next execution time
        return tcUpdatePeriod;
    }
};
