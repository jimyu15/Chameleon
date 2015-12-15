#include "Chameleon.h"
#define DEBUG
//#define ADXL335
#define ADXL345

volatile uint8_t state = SLEEP, last_state = 0, blink_num = 0, play_task = 0;
uint16_t move_event = 0;
uint16_t count = 0, state_count = 0;
uint8_t pos = 0, move = 0, double_click = 0;
uint16_t fcolor[MAX_COLOR] = {
  0};
uint16_t tcolor[MAX_COLOR] = {
  0};
uint16_t last_color, current_color;
uint8_t glow_count = 0, glow_limit = 40, glow_rate = 60, brightness = 255;

volatile uint8_t pwm = 0;

void setup()
{
  pin_init();
  led_init();
  imu_init();
  timer_init();
  //tcs_init();
  randomSeed(analogRead(6) * analogRead(7));
  
  //load();
  if (1)
    insert_color(tcolor, 0xFFFF, (int)glow_rate * 2 / 3);

#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void loop()
{

#ifndef DEBUG
  charging();
#endif

  update_pos();

  update_stat();

  blink();
  glow();
  //Serial.print("state: ");


  /*
  Serial.print("\tcount: ");
   Serial.print(count);
   Serial.print("\tstate count: ");
   Serial.print(state_count);
   
   
   Serial.print("\ttarget color:");
   Serial.print(tcolor[0], HEX);
   Serial.print("\tcurrent color:");
   Serial.print(current_color, HEX);
   Serial.print("\tglow count: ");
   Serial.print(glow_count);
   Serial.print("\tglow limit: ");
   Serial.print(glow_limit);
   */
  //
  /*
  Serial.print(state);
   Serial.print("\t");
   
   Serial.print(pos);
   Serial.print("\t");
   */
   Serial.print(imu_pos());
   Serial.print("\t");
  Serial.println(state);

  //Serial.println();

  if (state != SLEEPING && millis() / 1000 % 10 == 0)
  {
    //save();
  }
  while (millis() % 25)
    LPM0;



}

void blink()
{
  if (pos == TOP)
  {
    blink_num &= ~(R_LED + L_LED + S_LED + F_LED);
    return;
  }
  uint8_t temp = blink_num;
  if (millis() / 500 % 2)
  {
    if (temp & R_BLINK)
      temp |= R_LED;
    if (temp & L_BLINK)
      temp |= L_LED;
    if (temp & F_BLINK)
      temp |= F_LED;
    if (temp & S_BLINK)
      temp |= S_LED;
  }
  else
  {
    if (temp & R_BLINK)
      temp &= ~R_LED;
    if (temp & L_BLINK)
      temp &= ~L_LED;
    if (temp & F_BLINK)
      temp &= ~F_LED;
    if (temp & S_BLINK)
      temp &= ~S_LED;
  }
  blink_num = temp;


}

void update_pos()
{
  count++;
  uint8_t new_pos = imu_pos();
  if (move >= 3 && move <= 6 && count < 10)
  {
    pos = new_pos;
    return;
  }
  if (pos != new_pos && new_pos < 7 && new_pos > 0)
  {
    count = 0;

    if (new_pos > 0 && new_pos < 5 && pos > 0 && pos < 5)
    {
      if (((new_pos + 4) - pos) % 4 == 1)
        move = CW;
      else if (((new_pos + 4) - pos) % 4 == 3)
        move = CCW;
    }
    else
      move = MOVING;
    pos = new_pos;
  }

  if (new_pos > 6 && new_pos < 10)
  {
    if (move_event % 10 != new_pos)
      move_event = new_pos + 10;
    else
      move_event += 10;
  }
  else if (move_event)
  {
    if (move_event % 10 == FREEFALL && move_event > 80)
      move = FALL;
    else if (move_event % 10 == BTN && move_event > 200)
      move = PRESS;
    else if (move_event % 10 == BTN && move_event < 200)
    {
      if (double_click > 5)
      {
        double_click = 0;
        move_event = DCLICK;
      }
      else
      {
        move = CLICK;
        double_click = 1;
      }
    }
    move_event = 0;
  }

  if (move != CLICK && double_click)
  {
    double_click++;
    if (double_click > 30)
      double_click = 0;
  }

}

void update_stat()
{
  if (last_state != state)
  {
    move = 0;
    state_count = 0;
    last_state = state;
  }
  state_count++;



  switch (state)
  {
  case SLEEPING:
    if (state_count == 1)
    {
      //save();
      play_task = 0;
      insert_color(tcolor, 0, (int)glow_rate * 2 / 3);
      timer_pause();
      //imu_sleep();
      blink_num = 0;
    }
    if (glow_count >= glow_limit)
    {
      attachInterrupt(button, awake, FALLING);
      LPM4;
      detachInterrupt(button);
      state = IDLE;
      blink_num = 0;
      timer_resume();
      remove_color(tcolor, (int)glow_rate * 2 / 3);
      last_color = 0;
      glow_count = 0;
    }
    
    return;
    break;

  case IDLE://idle

    break;

  case MENU://menu
    if (state_count == 1)
      blink_num |= R_LED + F_LED + S_LED + L_LED;
    if (pos == FRONT && count > 10)
    {
      state = RANDOM;
    }
    else if (pos == BACK && count > 10)
    {
      state = FAVORITE;
    }
    else if (pos == LEFT && count > 10)
    {
      state = LAST;
    }
    else if (pos == RIGHT && count > 10)
    {
      state = SETTING;
    }
    else if (move == PRESS)
    {
      state = SLEEPING;
      return;
    }
    break;

  case RANDOM:
    if (state_count == 1)
    {
      blink_num = R_LED;
      play_task = 0;
    }
    if ((move == CW || move == CCW) && state_count < 100)
    {
      state = RANDOMIZE;
    }
    else if (pos == FRONT && state_count == 100)
    {
      play_task = RANDOM;
      blink_num = R_BLINK;
    }

    break;

  case RANDOMIZE:
    if (state_count == 1)
    {
      play_task = 0;
      insert_color(tcolor, random(1, 0xFFFF), glow_rate / 3);
      blink_num &= ~R_LED;
    }

    if (state_count == 10)
    {
      blink_num |= R_LED;
    }

    if (move == CW || move == CCW)
    {
      last_state = RANDOM;
    }

    break;

  case RANDOMING:
    if (state_count == 1)
    {
      insert_color(tcolor, random(1, 0xFFFF), glow_rate / 3 * 2);
    }
    else if (state_count == 80)
    {
      state_count = 0;
    }

    break;

  case FAVORITE:
    if (state_count == 1)
    {
      blink_num = F_LED;
      insert_color(tcolor, fcolor[0], glow_rate / 3);
      play_task = 0;
    }

    if (state_count == 110)
      blink_num |= F_LED;

    if (move == CW)
    {
      state_count = 101;
      next_color(fcolor, 0);
      insert_color(tcolor, fcolor[0], glow_rate / 3);
      move = 0;
      play_task = 0;
      blink_num = 0;
    }
    else if (move == CCW)
    {
      state_count = 101;
      prev_color(fcolor, 0);
      insert_color(tcolor, fcolor[0], glow_rate / 3);
      move = 0;
      play_task = 0;
      blink_num = 0;
    }
    else if (move == PRESS)
    {
      state_count = 101;
      remove_color(fcolor, 0);
      remove_color(tcolor, 0);
      insert_color(tcolor, fcolor[0], glow_rate / 3);
      last_color = 0;
      move = 0;
      play_task = 0;
    }
    else if (pos == BACK && state_count == 100)
    {
      play_task = FAVORITE;
      blink_num = F_LED + F_BLINK;
    }

    break;


  case LAST:
    if (state_count == 1)
    {
      blink_num = L_LED;
      play_task = 0;
    }
    if (state_count == 110)
      blink_num |= L_LED;
    if (move == CW)
    {
      state_count = 101;
      next_color(tcolor, glow_rate / 3);
      move = 0;
      blink_num = 0;
    }
    else if (move == CCW)
    {
      state_count = 101;
      prev_color(tcolor, glow_rate / 3);
      move = 0;
      blink_num = 0;
    }
    else if (move == PRESS)
    {
      state_count = 101;
      remove_color(tcolor, glow_rate / 3);
      last_color = 0;
      move = 0;
      blink_num = 0;
    }
    break;

  case SETTING:
    if (state_count == 1)
    {
      blink_num = S_LED;
    }
    if (state_count == 110)
      blink_num |= S_LED;
    if (move == CW)
    {
      state_count = 101;
      if (play_task)
        glow_rate = max(20, glow_rate - 10);
      else
        brightness = max(32, brightness - 32);
      move = 0;
      blink_num = 0;
    }
    else if (move == CCW)
    {
      state_count = 101;
      if (play_task)
        glow_rate = min(255, glow_rate + 10);
      else
        brightness = min(255, brightness + 32);
      move = 0;
      blink_num = 0;
    }
    break;

  case CHAM:
    if (state_count == 1)
    {
      //tcs_enable();
      play_task = 0;
      blink_num = 0;
      insert_color(tcolor, 0xFFFF, 1);
    }
    //tcolor[0] = tcs_read();

    if (state_count == 5000)
    {
      //tcs_disable();
    }
    break;




  default:
    break;


  }

  if (pos == BOT && count > 10)
  {
    state = MENU;
  }

  if (move == FALL && state != SLEEP)
  {
    state = CHAM;
  }

  if (move == CLICK && fcolor[0] != tcolor[0])
  {
    insert_color(fcolor, current_color, glow_rate / 6);
    last_color = 0xFFFF;
    move = 0;
  }
}

void glow()
{
  if (glow_count < glow_rate) 
    glow_count++;
  else
  {
    switch (play_task)
    {
    case RANDOM:
      insert_color(tcolor, random(0, 0xFFFF), (int)glow_rate * 2 / 3);
      break;
    case FAVORITE:
      next_color(fcolor, 0);
      insert_color(tcolor, fcolor[0], (int)glow_rate * 2 / 3);
      break;
    case LAST:
      next_color(tcolor, (int)glow_rate * 2 / 3);
      break;
    default:
      break;
    }
  }

  uint8_t rgb[2][3];
  rgb[0][0] = last_color >> 11 << 3;
  rgb[0][1] = ((last_color >> 5) & 0x3F) << 2;
  rgb[0][2] = (last_color & 0x1F) << 3;
  rgb[1][0] = tcolor[0] >> 11 << 3;
  rgb[1][1] = ((tcolor[0] >> 5) & 0x3F) << 2;
  rgb[1][2] = (tcolor[0] & 0x1F) << 3;
  for (int i = 0; i < 3; i++)
  {

    rgb[0][i] = ((float)rgb[0][i] + (rgb[1][i] - rgb[0][i]) * min(glow_count, glow_limit) / glow_limit);

  }
  current_color = (rgb[0][0] >> 3 << 11) + (rgb[0][1] >> 2 << 5) + (rgb[0][2] >> 3);
  led_write(rgb[0]);




}

void insert_color(uint16_t color[], uint16_t new_color, uint8_t limit)
{
  for (int i = MAX_COLOR - 1; i > 0; i--)
    color[i] = color[i - 1];
  last_color = color[0];
  color[0] = new_color;
  if (limit > 0) 
    glow_limit = limit;
  glow_count = 0;
}

void next_color(uint16_t color[], uint8_t limit)
{
  uint8_t num = MAX_COLOR - 1;
  while (color[num] == 0 && num > 0)
    num--;
  uint16_t temp_color = color[num];
  for (int i = num; i > 0; i--)
    color[i] = color[i - 1];
  last_color = color[0];
  color[0] = temp_color;
  if (limit > 0) 
    glow_limit = limit;
  glow_count = 0;
}

void prev_color(uint16_t color[], uint8_t limit)
{
  uint8_t num = MAX_COLOR - 1;
  while (color[num] == 0 && num > 0)
    num--;
  last_color = color[0];
  for (int i = 0; i < num; i++)
    color[i] = color[i + 1];
  color[num] = last_color;
  if (limit > 0) 
    glow_limit = limit;
  glow_count = 0;

}

void remove_color(uint16_t color[], uint8_t limit)
{
  uint8_t num = MAX_COLOR - 1;
  while (color[num] == 0 && num > 0)
    num--;
  for (int i = 0; i < num; i++)
    color[i] = color[i + 1];
  color[num] = 0;
  if (limit > 0) 
    glow_limit = limit;
  glow_count = 0;

}

void charging()
{
  if (state == CHAM)
    return;
  uint8_t check = check_bat();
  switch (check)
  {
  case 2:
    analogWrite(led_pin[4], gammatable[abs(100 - millis() / 10 % 200)]);
    break;
  case 1:
    analogWrite(led_pin[4], 100);
    break;
  default:
    digitalWrite(led_pin[4], LOW);
    break;
  }
}








