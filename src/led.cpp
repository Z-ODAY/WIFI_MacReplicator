/*
 * @Author: Z丶平凡
 * @Date: 2024-01-30 14:25:07
 * @LastEditTime: 2024-01-31 20:06:10
 * @LastEditors: Z丶平凡
 * @Description: In User Settings Edit
 * @FilePath: \WIFI_MacReplicator\src\led.cpp
 */

#include <Arduino.h>
#include <led.h>
#include <FastLED.h>



#define NUM_LEDS 1

#define DATA_PIN 48
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS];



void LedInit()
{
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  leds[0] = CRGB::Black;
  FastLED.setBrightness(20);
  FastLED.show();
}



void LedCountrol(uint8_t NewStatus)
{
  if (NewStatus)
  {
    leds[0] = CRGB::Green;
    FastLED.setBrightness(20);
    FastLED.show();
  }
  else
  {
    leds[0] = CRGB::Black;
    FastLED.setBrightness(20);
    FastLED.show();
  }
}



void RerstCompleteLed()
{
    leds[0] = CRGB::Red;
    FastLED.setBrightness(20);
    FastLED.show();
}



void TwinkleLed(uint16_t Mdelay,uint16_t TwinkleNum)
{
  static uint8_t Toggle = 0;

  if (TwinkleNum)
  {
    for (uint8_t i = 0; i < TwinkleNum; i++)
    {
      if (Toggle == 0)
      {
        LedCountrol(1);
      }
      else
      {
        LedCountrol(0);
      }
      Toggle =~Toggle;
      delay(Mdelay);
    }
  }
  else
  {
    if (Toggle == 0)
    {
      LedCountrol(1);
    }
    else
    {
      LedCountrol(0);
    }
    Toggle =~Toggle;
    delay(Mdelay);
  }
}


