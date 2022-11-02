#include <Arduino.h>
#include <SPI.h>

#include <ADS8167.h>

ADS8167 adc;

int main(){
    // cs, ready, alert
    Serial.begin(115200);
    while(!Serial);
    adc.init(&SPI, 36, 27);
    adc.setAllInputsSeparate();
    adc.enableOTFMode();

    while(1) {
        Serial.print("  adc0:   ");
        Serial.print(adc.readChannelOTF(1));
        Serial.print("  adc1:   ");
        Serial.print(adc.readChannelOTF(2));
        Serial.print("  adc2 :    ");
        Serial.print(adc.readChannelOTF(3));
        Serial.print("  adc3:   ");
        Serial.print(adc.readChannelOTF(4));
        Serial.print("  adc4: 	");
        Serial.print(adc.readChannelOTF(5));
        Serial.print("  adc5:   ");
        Serial.print(adc.readChannelOTF(6));
        Serial.print("  adc6: ");
        Serial.print(adc.readChannelOTF(7));
        Serial.print("  adc7:   ");
        Serial.println(adc.readChannelOTF(0));
        delay(200);
    }
    return 0;
}