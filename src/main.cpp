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
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

    Comms::initComms();
    Comms::registerCallback(220, ledPacketHandler);
}

unsigned long previousMillis = 0;  
const long interval = 10; 

bool capConnected = false;
int startHue = 0;

void loop()
{
    ArduinoOTA.handle();
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        Comms::processWaitingPackets();
        if(!capConnected){
            fill_rainbow(leds, NUM_LEDS, startHue, 255/NUM_LEDS);
            startHue += 1;
            startHue = startHue%256;
            FastLED.show();
            
        }
    }
}

void ledPacketHandler(Comms::Packet tmp) {
    capConnected = true;
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
      for(int i = full_leds; i< NUM_LEDS; i++){
        leds[i] = CRGB(0,0,0);
      }
      float overflow = scaledLED - full_leds;
      leds[full_leds] = CRGB(scale8(led_color.r, overflow * 256), scale8(led_color.g, overflow * 256), scale8(led_color.b, overflow * 256));
    }
    blur1d(leds, NUM_LEDS, 172); 
    blur1d(leds, NUM_LEDS, 172); 
    blur1d(leds, NUM_LEDS, 172); 
    blur1d(leds, NUM_LEDS, 172); 
    FastLED.show();
}
