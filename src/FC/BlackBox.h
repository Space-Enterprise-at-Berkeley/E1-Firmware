// #pragma once

// #include <NativeEthernet.h>
// #include <NativeEthernetUdp.h>
// #include <LittleFS.h>
// #include "Comms.h"

// namespace BlackBox {
//     void init();
//     void writePacket(Comms::Packet packet);
//     bool hasData();
//     void erase(Comms::Packet packet);
//     void writeBuffer();
//     void beginWrite();

//     /**
//      * @brief Prints data over Ethernet and Serial.
//      * 
//      */
//     void getData(Comms::Packet packet);

//     uint32_t reportBlackBoxStatus();
// }