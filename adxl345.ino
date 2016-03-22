#ifdef ADXL345
#include <Wire.h>

const uint8_t IMU_Address = 0xA7 >> 1; // ADXL345 的 I2C 地址
extern int16_t axis_offset[3] = {0};
float len[10];
float G = 1;


void imu_init()
{
  
  Wire.begin(); 
  setReg(ADXL345_REG_POWER_CTL, 0x08); //Power on
  setReg(ADXL345_REG_BW_RATE, 0x09);   //Set 50Hz data rate
  setReg(ADXL345_REG_DATA_FORMAT, 0x0B);
  delay(1);
  if (axis_offset[0] == 0 || axis_offset[1] == 0 || axis_offset[2] == 0)
    imu_cal();
}

void imu_sleep()
{
  setReg(ADXL345_REG_POWER_CTL, 0);
}

void imu_cal()
{
  long sum[3] = {0};
  for (int i = 0; i < 100; i++)
  {
    sum[0] += get_X();
    sum[1] += get_Y();
    sum[2] += (get_Z() + 255);
    delay(20);
  }
  for (int i = 0; i < 3; i++)
  {
    axis_offset[i] = sum[i] / 100;
  }
}

void imu_read(float raw[])
{
  raw[0] = (float)(get_X() - axis_offset[0]) / 300;
  raw[1] = (float)(get_Y() - axis_offset[1]) / 300;
  raw[2] = (float)(get_Z() - axis_offset[2]) / 256;
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
  if (!digitalRead(button))
    return BTN;
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

int16_t get_X()
{
  return imu_read16(ADXL345_REG_DATAX0);
}

int16_t get_Y()
{
  return imu_read16(ADXL345_REG_DATAY0);
}

int16_t get_Z()
{
  return imu_read16(ADXL345_REG_DATAZ0);
}


void setReg(uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(IMU_Address);
  Wire.write(reg); 
  Wire.write(data); 
  Wire.endTransmission();
}

uint8_t getReg(uint8_t reg)
{
  Wire.beginTransmission(IMU_Address);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(IMU_Address, 1);
  return Wire.read();

}

int16_t imu_read16(uint8_t reg)
{
  Wire.beginTransmission(IMU_Address);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(IMU_Address, 2);
  return (int16_t)(Wire.read() | (Wire.read() << 8));
}
#endif

