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
    fill_solid(leds, NUM_LEDS, led_color);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.show();

    Comms::initComms();
    Comms::registerCallback(220, ledPacketHandler);
}

unsigned long previousMillis = 0;  
const long interval = 10; 

void loop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        ArduinoOTA.handle();
        Comms::processWaitingPackets();
    }
}

void ledPacketHandler(Comms::Packet tmp) {
    float capValue = Comms::packetGetFloat(&tmp, 0);
    Serial.println(capValue);

    float scaledLED = (capValue - BOTTOM_LIMIT)/(TOP_LIMIT - BOTTOM_LIMIT) * NUM_LEDS;
    if(scaledLED < 0) scaledLED = 0;
    if(scaledLED > NUM_LEDS) scaledLED = NUM_LEDS;

    int full_leds = floor(scaledLED);
    fill_solid(leds, full_leds, led_color);
    fill_solid(leds + full_leds, NUM_LEDS - full_leds, CRGB(0,0,0));

    float overflow = scaledLED - full_leds;
    leds[full_leds] = led_color;
    leds[full_leds].nscale8(overflow * 256);

    blur1d(leds, NUM_LEDS, 172); 
    blur1d(leds, NUM_LEDS, 172); 
    blur1d(leds, NUM_LEDS, 172); 
    blur1d(leds, NUM_LEDS, 172); 

    FastLED.show();
}
