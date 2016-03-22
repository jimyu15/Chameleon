#include <Wire.h>
#include "Adafruit_TCS34725.h"
#define clear_threshold 0xA0
uint8_t led_pwm = 0;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);

void tcs_init()
{
  tcs.begin();
  //tcs.setInterrupt(1);
  //tcs.setIntLimits(0x0033, 0x3333);
  pinMode(int_pin, INPUT_PULLUP);
}

void tcs_enable()
{
  tcs.enable();
}

void tcs_disable()
{
  tcs.disable();
  digitalWrite(led_pin[4], LOW);
}

uint16_t tcs_read() {
  uint16_t clear, red, green, blue;
  tcs.getRawData(&red, &green, &blue, &clear);
  if (clear < clear_threshold)
    digitalWrite(led_pin[4], HIGH);

  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  
  red = (int)r / 8;
  green = (int)g / 4;
  blue = (int)b / 8;
  
  return (red << 11) + (green << 5) + blue; 
}
