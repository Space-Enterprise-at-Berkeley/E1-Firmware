#include "Actuators.h"

namespace Actuators {

    // TODO: change this to appropriate value
    uint32_t actuatorCheckPeriod = 50 * 1000;

    // TODO: set correct telem packet IDs
    Comms::Packet act1Packet = {.id = 70};
    uint8_t act1State = 0;
    float act1Voltage = 0.0;
    float act1Current = 0.0;
    Task *stop1;
    
    Comms::Packet act2Packet = {.id = 71};
    uint8_t act2State = 0;
    float act2Voltage = 0.0;
    float act2Current = 0.0;
    Task *stop2;

    Comms::Packet act3Packet = {.id = 72};
    uint8_t act3State = 0;
    float act3Voltage = 0.0;
    float act3Current = 0.0;
    Task *stop3;

    Comms::Packet act4Packet = {.id = 73};
    uint8_t act4State = 0;
    float act4Voltage = 0.0;
    float act4Current = 0.0;
    Task *stop4;

    Comms::Packet act5Packet = {.id = 74};
    uint8_t act5State = 0;
    float act5Voltage = 0.0;
    float act5Current = 0.0;
    Task *stop5;

    Comms::Packet act6Packet = {.id = 75};
    uint8_t act6State = 0;
    float act6Voltage = 0.0;
    float act6Current = 0.0;
    Task *stop6;

    Comms::Packet act7Packet = {.id = 76};
    uint8_t act7State = 0;
    float act7Voltage = 0.0;
    float act7Current = 0.0;
    Task *stop7;

    void driveForwards(uint8_t pin1, uint8_t pin2, uint8_t *actState, uint8_t actuatorID){
        digitalWriteFast(pin1, HIGH);
        digitalWriteFast(pin2, LOW);
        *actState = 1;
    }

    void driveBackwards(uint8_t pin1, uint8_t pin2, uint8_t *actState, uint8_t actuatorID){
        digitalWriteFast(pin1, LOW);
        digitalWriteFast(pin2, HIGH);
        *actState = 2;
    }

    void stopAct(uint8_t pin1, uint8_t pin2, uint8_t *actState, uint8_t actuatorID){
        digitalWriteFast(pin1, LOW);
        digitalWriteFast(pin2, LOW);
        *actState = 0;
    }

    void brakeAct(uint8_t pin1, uint8_t pin2, uint8_t *actState, uint8_t actuatorID){
        digitalWriteFast(pin1, HIGH);
        digitalWriteFast(pin2, HIGH);
        *actState = 4; // Probably won't brake
    }

    void extendAct1(){ driveForwards(act1Pin1, act1Pin2, &act1State, 0); }
    void retractAct1(){ driveBackwards(act1Pin1, act1Pin2, &act1State, 0); }
    uint32_t stopAct1(){ stopAct(act1Pin1, act1Pin2, &act1State, 0); stop1->enabled = false; return 0;}
    void brakeAct1(){ brakeAct(act1Pin1, act1Pin2, &act1State, 0); }
    void act1PacketHandler(Comms::Packet tmp, uint8_t ip){ actPacketHandler(tmp, &extendAct1, &retractAct1, stop1); }

    void extendAct2(){ driveForwards(act2Pin1, act2Pin2, &act2State, 1); }
    void retractAct2(){ driveBackwards(act2Pin1, act2Pin2, &act2State, 1); }
    uint32_t stopAct2(){ stopAct(act2Pin1, act2Pin2, &act2State, 1); stop2->enabled = false; return 0;}
    void brakeAct2(){ brakeAct(act2Pin1, act2Pin2, &act2State, 1); }
    void act2PacketHandler(Comms::Packet tmp, uint8_t ip){ actPacketHandler(tmp, &extendAct2, &retractAct2, stop2); }

    void extendAct3(){ driveForwards(act3Pin1, act3Pin2, &act3State, 2); }
    void retractAct3(){ driveBackwards(act3Pin1, act3Pin2, &act3State, 2); }
    uint32_t stopAct3(){ stopAct(act3Pin1, act3Pin2, &act3State, 2); stop3->enabled = false; return 0;}
    void brakeAct3(){ brakeAct(act3Pin1, act3Pin2, &act3State, 2); }
    void act3PacketHandler(Comms::Packet tmp, uint8_t ip){ actPacketHandler(tmp, &extendAct3, &retractAct3, stop3); }

    void extendAct4(){ driveForwards(act4Pin1, act4Pin2, &act4State, 3); }
    void retractAct4(){ driveBackwards(act4Pin1, act4Pin2, &act4State, 3); }
    uint32_t stopAct4(){ stopAct(act4Pin1, act4Pin2, &act4State, 3); stop4->enabled = false; return 0;}
    void brakeAct4(){ brakeAct(act4Pin1, act4Pin2, &act4State, 3); }
    void act4PacketHandler(Comms::Packet tmp, uint8_t ip){ actPacketHandler(tmp, &extendAct4, &retractAct4, stop4); }

    void extendAct5(){ driveForwards(act5Pin1, act5Pin2, &act5State, 4); }
    void retractAct5(){ driveBackwards(act5Pin1, act5Pin2, &act5State, 4); }
    uint32_t stopAct5(){ stopAct(act5Pin1, act5Pin2, &act5State, 4); stop5->enabled = false; return 0;}
    void brakeAct5(){ brakeAct(act5Pin1, act5Pin2, &act5State, 4); }
    void act5PacketHandler(Comms::Packet tmp, uint8_t ip){ actPacketHandler(tmp, &extendAct5, &retractAct5, stop5); }

    void extendAct6(){ driveForwards(act6Pin1, act6Pin2, &act6State, 5); }
    void retractAct6(){ driveBackwards(act6Pin1, act6Pin2, &act6State, 5); }
    uint32_t stopAct6(){ stopAct(act6Pin1, act6Pin2, &act6State, 5); stop6->enabled = false; return 0;}
    void brakeAct6(){ brakeAct(act6Pin1, act6Pin2, &act6State, 5); }
    void act6PacketHandler(Comms::Packet tmp, uint8_t ip){ actPacketHandler(tmp, &extendAct6, &retractAct6, stop6); }

    void extendAct7(){ driveForwards(act7Pin1, act7Pin2, &act7State, 6); }
    void retractAct7(){ driveBackwards(act7Pin1, act7Pin2, &act7State, 6); }
    uint32_t stopAct7(){ stopAct(act7Pin1, act7Pin2, &act7State, 6); stop7->enabled = false; return 0;}
    void brakeAct7(){ brakeAct(act7Pin1, act7Pin2, &act7State, 6); }
    void act7PacketHandler(Comms::Packet tmp, uint8_t ip){ actPacketHandler(tmp, &extendAct7, &retractAct7, stop7); }

    void actPacketHandler(Comms::Packet tmp, void (*extend)(), void (*retract)(), Task *stopTask){
/*         switch(tmp.data[0]){
            case 0:
                (*extend)();
                break;
            case 1:
                (*retract)();
                break;
            case 2:
                (*extend)();
                if(stopTask->enabled) stopTask->nexttime += Comms::packetGetUint32(&tmp, 1) * 1000;
                else stopTask->nexttime = micros() + Comms::packetGetUint32(&tmp, 1) * 1000;
                stopTask->enabled = true;
                break;
            case 3:
                (*retract)();
                if(stopTask->enabled) stopTask->nexttime += Comms::packetGetUint32(&tmp, 1) * 1000;
                else stopTask->nexttime = micros() + Comms::packetGetUint32(&tmp, 1) * 1000;
                stopTask->enabled = true;
                break;
        } */

        // (Actuate code) 0: extend fully 1: retract fully 2: extend millis 3: retract millis

        if(tmp.data[0]%2)(*extend)();
        else (*retract)();

        if(tmp.data[0]>1){
            uint32_t actuatetime = Comms::packetGetUint32(&tmp, 1);
            if(stopTask->enabled) stopTask->nexttime += actuatetime * 1000;
            else stopTask->nexttime = micros() + actuatetime * 1000;
            stopTask->enabled = true;
        }
    }

    void sampleActuator(Comms::Packet *packet, INA219 *ina, float *voltage, float *current, uint8_t *actState, uint8_t actuatorID) {
        *voltage = ina->readBusVoltage();
        *current = ina->readShuntCurrent();

        if (*current > OClimits[actuatorID]){
            switch(actuatorID){
                case 0: stopAct1(); break;
                case 1: stopAct2(); break;
                case 2: stopAct3(); break;
                case 3: stopAct4(); break;
                case 4: stopAct5(); break;
                case 5: stopAct6(); break;
                case 6: stopAct7(); break;
            }
            *actState = 3;
        }

        if ((*actState == 1 || *actState == 2) && *current < stopCurrent){
            switch(actuatorID){
                case 0: stopAct1(); break;
                case 1: stopAct2(); break;
                case 2: stopAct3(); break;
                case 3: stopAct4(); break;
                case 4: stopAct5(); break;
                case 5: stopAct6(); break;
                case 6: stopAct7(); break;
            }
        }

        packet->len = 0;
        Comms::packetAddUint8(packet, *actState);
        Comms::packetAddFloat(packet, *voltage);
        Comms::packetAddFloat(packet, *current);
        Comms::emitPacket(packet);
    }

    uint32_t act1Sample() {
        sampleActuator(&act1Packet, &HAL::chan4, &act1Voltage, &act1Current, &act1State, 0);
        return actuatorCheckPeriod;
    }

    uint32_t act2Sample() {
        sampleActuator(&act2Packet, &HAL::chan5, &act2Voltage, &act2Current, &act2State, 1);
        return actuatorCheckPeriod;
    }

    uint32_t act3Sample() {
        sampleActuator(&act3Packet, &HAL::chan6, &act3Voltage, &act3Current, &act3State, 2);
        return actuatorCheckPeriod;
    }

    uint32_t act4Sample() {
        sampleActuator(&act4Packet, &HAL::chan7, &act4Voltage, &act4Current, &act4State, 3);
        return actuatorCheckPeriod;
    }

    uint32_t act5Sample() {
        sampleActuator(&act5Packet, &HAL::chan8, &act5Voltage, &act5Current, &act5State, 4);
        return actuatorCheckPeriod;
    }

    uint32_t act6Sample() {
        sampleActuator(&act6Packet, &HAL::chan9, &act6Voltage, &act6Current, &act6State, 5);
        return actuatorCheckPeriod;
    }

    uint32_t act7Sample() {
        sampleActuator(&act7Packet, &HAL::chan10, &act7Voltage, &act7Current, &act7State, 6);
        return actuatorCheckPeriod;
    }

    void initActuators() {
        Comms::registerCallback(170, act1PacketHandler);
        Comms::registerCallback(171, act2PacketHandler);
        Comms::registerCallback(172, act3PacketHandler);
        Comms::registerCallback(173, act4PacketHandler);
        Comms::registerCallback(174, act5PacketHandler);
        Comms::registerCallback(175, act6PacketHandler);
        Comms::registerCallback(176, act7PacketHandler);
    }
};