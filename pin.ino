void pin_init()
{
  for (int i = 0; i < 5; i++)
  {
    pinMode(led_pin[i], OUTPUT);
    digitalWrite(led_pin[i], LOW);
  }
  pinMode(tin, OUTPUT);
  digitalWrite(tin, LOW);
  pinMode(tout, INPUT);
}

uint8_t check_bat()
{
  uint8_t test;
  digitalWrite(tin, LOW);
  test = digitalRead(tout) * 2;
  digitalWrite(tin, HIGH);
  test += digitalRead(tout);
  switch(test)
  {
    case 0:
      return 2;
    case 1:
      return 1;
    default:
      return 0;
  }
}
