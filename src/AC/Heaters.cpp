#include "Heaters.h"

namespace Heaters {

    // TODO: change this to appropriate value
    uint32_t heaterCheckPeriod = 50 * 1000;

    // TODO: set correct telem packet IDs
    Comms::Packet ctl12vChan1Packet = {.id = 0};
    float ctl12vChan1Voltage = 0.0;
    float ctl12vChan1Current = 0.0;

    Comms::Packet ctl12vChan2Packet = {.id = 0};
    float ctl12vChan2Voltage = 0.0;
    float ctl12vChan2Current = 0.0;

    Comms::Packet ctl24vChan1Packet = {.id = 0};
    float ctl24vChan1Voltage = 0.0;
    float ctl24vChan1Current = 0.0;

    Comms::Packet ctl24vChan2Packet = {.id = 0};
    float ctl24vChan2Voltage = 0.0;
    float ctl24vChan2Current = 0.0;

    void sampleHeater(Comms::Packet *packet, INA219 *ina, float *voltage, float *current) {
        *voltage = ina->readBusVoltage();
        *current = ina->readShuntCurrent();

        packet->len = 0;
        Comms::packetAddFloat(packet, *voltage);
        Comms::packetAddFloat(packet, *current);
        Comms::emitPacket(packet);
    }

    uint32_t ctl12vChan1Sample() {
        sampleHeater(&ctl12vChan1Packet, &HAL::chan0, &ctl12vChan1Voltage, &ctl12vChan1Current);
        return heaterCheckPeriod;
    }

    uint32_t ctl12vChan2Sample() {
        sampleHeater(&ctl12vChan2Packet, &HAL::chan1, &ctl12vChan2Voltage, &ctl12vChan2Current);
        return heaterCheckPeriod;
    }

    uint32_t ctl24vChan1Sample() {
        sampleHeater(&ctl24vChan1Packet, &HAL::chan2, &ctl24vChan1Voltage, &ctl24vChan1Current);
        return heaterCheckPeriod;
    }
    uint32_t ctl24vChan2Sample() {
        sampleHeater(&ctl24vChan2Packet, &HAL::chan3, &ctl24vChan2Voltage, &ctl24vChan2Current);
        return heaterCheckPeriod;
    }

    void initHeaters() {
        
    }
};