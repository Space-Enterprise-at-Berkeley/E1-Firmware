// #include "BlackBox.h"

// namespace BlackBox {

//     const char* filePath = "data.txt";

//     Comms::Packet blackBoxPacket = {.id = 30};

//     uint32_t bbUpdatePeriod = 100 * 1000;

//     LittleFS_QSPIFlash blackBox;

//     EthernetUDP Udp;

//     int bufferIndex = 0;
//     uint8_t buffer[256] = {0};

//     bool writeEnabled = false;

//     void init() {
//         DEBUG("Initializing black box\n");
//         Comms::registerEmitter(&writePacket);
//         // Comms::registerCallback(153, &getData);
//         Comms::registerCallback(29, &beginWrite); // Begin writing to flash chip when Flight Mode packet sent
//         Comms::registerCallback(153, &erase);

//         blackBox.begin();

//         //check for previous data, else initialize data
//         bool hasData = blackBox.exists(filePath);

//         if(!hasData) {
//             DEBUG("BLACKBOX: NO PREVIOUS DATA DETECTED");
//             DEBUG("\n");
//         } else {
//             DEBUG("BLACKBOX: PREVIOUS DATA DETECTED");
//             DEBUG("\n");
//         }
//     }

//     void beginWrite() {
//         DEBUG("BLACKBOX: WRITE ENABLED");
//         writeEnabled = true;
//     }

//     uint32_t reportBlackBoxStatus() {
//         uint32_t storageUsed = blackBox.usedSize();

//         Comms::packetAddUint32(&blackBoxPacket, storageUsed);
//         Comms::packetAddUint8(&blackBoxPacket, writeEnabled);
//         Comms::emitPacket(&blackBoxPacket);
//         // return the next execution time
//         return bbUpdatePeriod;

//     }

//     void writePacket(Comms::Packet packet) {
//         if (writeEnabled && blackBox.usedSize() + 50 <= blackBox.totalSize()) {

//             //check if buffer full
//             if (bufferIndex + 8 + packet.len > 256) {
//                 DEBUG("WRITING BYTES TO BOX");
//                 DEBUG("\n");
//                 writeBuffer();
//             }

//             DEBUG("WRITING PACKET TO BUFFER");
//             DEBUG("\n");

//             //write metadata
//             buffer[bufferIndex] = packet.id;
//             buffer[bufferIndex + 1] = packet.len;
//             buffer[bufferIndex + 2] = packet.timestamp[0];
//             buffer[bufferIndex + 3] = packet.timestamp[1];
//             buffer[bufferIndex + 4] = packet.timestamp[2];
//             buffer[bufferIndex + 5] = packet.timestamp[3];
//             buffer[bufferIndex + 6] = packet.checksum[1];
//             buffer[bufferIndex + 7] = packet.checksum[2];

//             bufferIndex += 8;

//             for (int i = 0; i < packet.len; i++) {
//                 buffer[bufferIndex] = packet.data[i];
//                 bufferIndex += 1;
//             }
//         }
//     }

//     /**
//      * @brief writes and clears the packet buffer
//      * 
//      */
//     void writeBuffer() {
//         File dataFile = blackBox.open(filePath, FILE_WRITE);
//         DEBUG("BLACKBOX: WRITING TO BLACKBOX");
//         DEBUG("\n");
//         dataFile.write(buffer, 256);
//         dataFile.close();
//         //reset buffer
//         std::fill_n(buffer, 256, 0);
//         bufferIndex = 0;
//     }

//     void erase(Comms::Packet packet) {
//         blackBox.remove(filePath);
//         writeEnabled = false;
//         DEBUG("BLACKBOX: DATA ERASED");
//         DEBUG("\n");
//     }

//     void getData(Comms::Packet packet) {
        
//         uint8_t readBuffer[256] = {0};
//         //UDP BEGIN
//         if (blackBox.exists(filePath)) {
//             File dataFile = blackBox.open(filePath, FILE_READ);
//             while(dataFile.read(readBuffer, 256) > 0) {
//                 for (int i = 0; i < 256; i++) {
//                     Serial.write(readBuffer[i]);
//                     // Udp.write(readBuffer[i]);
//                 }
//             }
//             // Udp.endPacket();
//             dataFile.close();
//             writeEnabled = false;
//         } else {
//             DEBUG("BLACKBOX: FILE DOESN'T EXIST");
//             DEBUG("\n");
//         }
//     }
// }