#include <Arduino.h>

#include <FastLED.h>

#include <Comms.h>
#include <Common.h>

#define LOX_BOT 100
#define LOX_TOP 125
#define FUEL_BOT 100
#define FUEL_TOP 125

#define NUM_LEDS 144
#define LOX_DATA_PIN 21
#define FUEL_DATA_PIN 20
#define NUM_LEDS2 300
#define DATA_PIN2 12

#ifdef LOX
CRGB led_color(0, 50, 98);
#else
CRGB led_color(100, 60, 0);
#endif

CRGB loxLed[NUM_LEDS];
CRGB fuelLed[NUM_LEDS];
CRGB leds2[NUM_LEDS2];

void loxLedPacketHandler(Comms::Packet tmp);
void fuelLedPacketHandler(Comms::Packet tmp);

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, LOX_DATA_PIN>(loxLed, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, FUEL_DATA_PIN>(fuelLed, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN2>(leds2, NUM_LEDS2);

  Comms::initComms();
  Comms::registerCallback(221, loxLedPacketHandler);
  Comms::registerCallback(222, fuelLedPacketHandler);
}

unsigned long previousMillis = 0;
const long interval = 10;

bool loxCapConnected = false;
bool fuelCapConnected = false;
int startHue = 0;

void loop()
{
  ArduinoOTA.handle();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    Comms::processWaitingPackets();
    startHue += 1;
    startHue = startHue % 256;
    if (!loxCapConnected)
    {
      fill_rainbow(loxLed, NUM_LEDS, startHue, 255 / NUM_LEDS);
    }
    if (!fuelCapConnected)
    {
      fill_rainbow(fuelLed, NUM_LEDS, startHue, 255 / NUM_LEDS);
    }
#ifdef LOX
    fill_rainbow(leds2, NUM_LEDS2, startHue, 255 / NUM_LEDS);
#endif
    FastLED.show();
  }
}

void loxLedPacketHandler(Comms::Packet tmp)
{
  loxCapConnected = true;
  float capValue = Comms::packetGetFloat(&tmp, 0);

  float scaledLED = (capValue - LOX_BOT) / (LOX_BOT - LOX_BOT) * NUM_LEDS;
  if (scaledLED < 0)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      loxLed[i] = CRGB(0, 0, 0);
    }
  }
  else if (scaledLED > NUM_LEDS)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      loxLed[i] = led_color;
    }
  }
  else
  {
    int full_leds = floor(scaledLED);
    for (int i = 0; i < full_leds; i++)
    {
      loxLed[i] = led_color;
    }
    for (int i = full_leds; i < NUM_LEDS; i++)
    {
      loxLed[i] = CRGB(0, 0, 0);
    }
    float overflow = scaledLED - full_leds;
    loxLed[full_leds] = CRGB(scale8(led_color.r, overflow * 256), scale8(led_color.g, overflow * 256), scale8(led_color.b, overflow * 256));
  }
  blur1d(loxLed, NUM_LEDS, 172);
  blur1d(loxLed, NUM_LEDS, 172);
  blur1d(loxLed, NUM_LEDS, 172);
  blur1d(loxLed, NUM_LEDS, 172);
  FastLED.show();
}

void fuelLedPacketHandler(Comms::Packet tmp)
{
  fuelCapConnected = true;
  float capValue = Comms::packetGetFloat(&tmp, 0);

  float scaledLED = (capValue - FUEL_BOT) / (FUEL_BOT - FUEL_BOT) * NUM_LEDS;
  if (scaledLED < 0)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      fuelLed[i] = CRGB(0, 0, 0);
    }
  }
  else if (scaledLED > NUM_LEDS)
  {
    for (int i = NUM_LEDS - 1; i >= 0; i--)
    {
      fuelLed[i] = led_color;
    }
  }
  else
  {
    int full_leds = floor(scaledLED);
    for (int i = NUM_LEDS - 1; i > NUM_LEDS - 1 - full_leds; i--)
    {
      fuelLed[i] = led_color;
    }
    for (int i = NUM_LEDS - 1 - full_leds; i >= 0; i--)
    {
      fuelLed[i] = CRGB(0, 0, 0);
    }
    float overflow = scaledLED - full_leds;
    fuelLed[NUM_LEDS - 1 - full_leds] = CRGB(scale8(led_color.r, overflow * 256), scale8(led_color.g, overflow * 256), scale8(led_color.b, overflow * 256));
  }
  blur1d(fuelLed, NUM_LEDS, 172);
  blur1d(fuelLed, NUM_LEDS, 172);
  blur1d(fuelLed, NUM_LEDS, 172);
  blur1d(fuelLed, NUM_LEDS, 172);
  FastLED.show();
}