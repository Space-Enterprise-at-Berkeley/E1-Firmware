#include <Arduino.h>

#include <FastLED.h>

#include <Comms.h>
#include <Common.h>
 
#define NUM_LEDS 144
#define DATA_PIN 21

#ifdef LOX
CRGB led_color(0, 30, 255);
#else 
CRGB led_color(102, 255, 102);
#endif

CRGB leds[NUM_LEDS];

void ledPacketHandler(Comms::Packet tmp);

void setup()
{
    Serial.begin(115200);
    Comms::initComms();

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    Comms::registerCallback(220, ledPacketHandler);
}

Comms::Packet capPacket = {.id = 220};

void loop()
{
    ArduinoOTA.handle();
    Comms::processWaitingPackets();
}

void ledPacketHandler(Comms::Packet tmp) {
    float capValue = Comms::packetGetFloat(&tmp, 0);
    float scaledLED = (capValue - BOTTOM_LIMIT)/(TOP_LIMIT - BOTTOM_LIMIT) * NUM_LEDS;
    if(scaledLED < 0){
      for(int i = 0; i< NUM_LEDS; i++){
        leds[i] = CRGB(0,0,0);
      }
    }else if(scaledLED > NUM_LEDS){
      for(int i = 0; i< NUM_LEDS; i++){
        leds[i] = led_color;
      }
    }else{
      int full_leds = floor(scaledLED);
      for(int i = 0; i< full_leds; i++){
        leds[i] = led_color;
      }
      float overflow = scaledLED - full_leds;
      leds[full_leds] = CRGB(scale8(led_color.r, overflow), scale8(led_color.g, overflow), scale8(led_color.b, overflow));
      
    }
    FastLED.show();
}
