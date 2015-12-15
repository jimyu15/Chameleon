#include <WS2811Driver.h>


WS2811Driver led;
const uint8_t blank[3] = {0x00, 0x00, 0x00};
  
void led_init()
{
  led.setLEDCount(1);
  led.setPinMask(BIT0);
  led.begin();
  led.write(blank);
  
}

void led_write(uint8_t color[])
{
  uint8_t rgb[3];
  for (int i = 0; i < 3; i++)
  {
    
    rgb[i] = gammatable[(int)((float)color[i] * brightness / 255)];
  
  }
  timer_pause();
  led.write(rgb);
  timer_resume();
}

