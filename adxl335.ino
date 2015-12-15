#ifdef ADXL335

#define sampling 5

const uint8_t imu_pin[3] = {5, 6, 7};
float len[10];
float G = 1;

void imu_init()
{
  for (int i = 0; i < 3; i++)
    pinMode(imu_pin[i], INPUT);
  while (G == 1)
    imu_pos();
  move = 0;
}

void imu_read(float raw[])
{
  for (int i = 0; i < 3; i++)
  {
    int data = 0;
    for (int j = 0; j < sampling; j++)
      data += analogRead(imu_pin[i]);
    data /= sampling;
    raw[i] = data - 512;
  }
  
}

float v_len(float v1[])
{
  float sum = 0;
  for (int i = 0; i < 3; i++)
    sum += v1[i] * v1[i];
  return sqrt(sum);
}
  

uint8_t imu_pos()
{
  float raw[3];
  imu_read(raw);
  
  float lmax = len[0], lmin = len[0], sum = 0;
  for (int i = 9; i > 0; i--)
  {
    len[i] = len[i - 1];
    if (len[i] > lmax)
      lmax = len[i];
    if (len[i] < lmin)
      lmin = len[i];
    sum += len[i];
  }
  if (lmax - lmin < 4)
    G = sum / 9;
  
  
  len[0] = v_len(raw);
  
  
  if (len[0] < G / 3)
    return FREEFALL;
  
  if (len[0] > G * 1.4)
    return THROW;
  
  
  for (int i = 0; i < 3; i++)
  {
    raw[i] /= len[0];
  }
  /*
  if ((len[0] < G * 0.9) && (raw[0] > 0.95 || raw[1] > 0.95 || raw[2] > 0.95 || raw[0] < -0.95 || raw[1] < -0.95 || raw[2] < -0.95))
    return HIT;
    */
  
  if (raw[0] > 0.98)
    return FRONT;
  else if (raw[1] > 0.98)
    return RIGHT;
  else if (raw[2] > 0.98)
    return TOP;
  else if (raw[0] < -0.98)
    return BACK;
  else if (raw[1] < -0.98)
    return LEFT;
  else if (raw[2] < -0.98)
    return BOT;
  return UNSTABLE;
}

#endif
