// #include "Power.h"

// namespace Power {

//     // TODO: change this to appropriate value
//     uint32_t powerUpdatePeriod = 100 * 1000;

//     Comms::Packet supply8Packet = {.id = 3};
//     float supply8Voltage = 0.0;
//     float supply8Current = 0.0;
//     float supply8Power = 0.0;

//     PCA9539 ioOCExpander(0x24);

//     void init() {
//         ioOCExpander.pinMode(1, INPUT); // Chute1
//         ioOCExpander.pinMode(2, INPUT); // Chute2
//         ioOCExpander.pinMode(3, INPUT); // Amp
//         ioOCExpander.pinMode(4, INPUT); // Cam1
//         ioOCExpander.pinMode(5, INPUT); // Cam2
//         ioOCExpander.pinMode(6, INPUT); // Radio
//         ioOCExpander.pinMode(7, INPUT); // Valve1
//         ioOCExpander.pinMode(8, INPUT); // Valve2
//         ioOCExpander.pinMode(9, INPUT); // Valve3
//         ioOCExpander.pinMode(10, INPUT); // Valve4
//         ioOCExpander.pinMode(11, INPUT); // Valve5
//         ioOCExpander.pinMode(12, INPUT); // Valve6
//         ioOCExpander.pinMode(13, INPUT); // HBridge1
//         ioOCExpander.pinMode(14, INPUT); // HBridge2
//         ioOCExpander.pinMode(15, INPUT); // HBridge3
//     }

//     uint32_t supply8Sample() {
//         DEBUG("pwr\n");
//         DEBUG_FLUSH();
//         // ioOCExpander.digitalReadAll();
//         supply8Voltage = HAL::supply8v.readBusVoltage();
//         DEBUG("pwr1\n");
//         DEBUG_FLUSH();
//         supply8Current = HAL::supply8v.readShuntCurrent();
//         DEBUG("pwr2\n");
//         DEBUG_FLUSH();
//         supply8Power = supply8Voltage * supply8Current;
//         supply8Packet.len = 0;
//         Comms::packetAddFloat(&supply8Packet, supply8Voltage);
//         Comms::packetAddFloat(&supply8Packet, supply8Current);
//         Comms::packetAddFloat(&supply8Packet, supply8Power);
//         Comms::emitPacket(&supply8Packet);
//         DEBUG("pwr done\n");
//         DEBUG_FLUSH();
//         return powerUpdatePeriod;
//     }
// }
