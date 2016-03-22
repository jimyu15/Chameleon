const uint8_t led_mask[4] = {BIT0, BIT5, BIT6, BIT7};
#define pin_mask B01111000
volatile uint8_t blink_count = 0;

void timer_init()
{
  TACCR0 = 31;
  TACCTL0 = CCIE;
  TACTL = MC_1 | ID_0 | TASSEL_1 | TACLR;
}

void timer_pause()
{
  TACCTL0 = 0;
}

void timer_resume()
{
  TACCTL0 = CCIE;
}

__attribute__((interrupt(TIMER0_A0_VECTOR)))
static void TA0_ISR(void)
{
  static uint8_t temp = P2OUT & pin_mask;
  P2OUT = temp + blink_num & led_mask[(blink_count++) % 4];
  //P2OUT = ((blink_count++ % 8) > (millis() / 100) % 8) ? 0 : led_mask[2];
}



