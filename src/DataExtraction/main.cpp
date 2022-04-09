#include <LittleFS.h>

#include <Arduino.h>

const char* filePath = "data.txt";
LittleFS_QSPIFlash blackBox;

int main() {
    Serial.begin(115200);
    while(!Serial);
    // Serial.println("begin");

    blackBox.begin();

    // Serial.println(blackBox.exists(filePath));
    // Serial.println(blackBox.totalSize());
    // Serial.flush();

    File dataFile = blackBox.open(filePath, FILE_READ);
    unsigned char b;
    while(dataFile.read(&b, 1) > 0) {
        Serial.write(b);
        Serial.flush();
    }

    while(1);

    return 0;
}
