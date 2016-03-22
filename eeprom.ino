#include <Wire.h>
#define rom_addr 0x50

void write8(uint8_t addr, uint8_t data)
{
  Wire.beginTransmission(rom_addr);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();
  delay(1);
}

void write16(uint8_t addr, uint16_t data)
{
  Wire.beginTransmission(rom_addr);
  Wire.write(addr);
  Wire.write((data >> 8) & 0xFF);
  Wire.write(data & 0xFF);
  Wire.endTransmission();
  delay(1);
}

void write16a(uint8_t addr, uint16_t data[], uint8_t num)
{
  Wire.beginTransmission(rom_addr);
  Wire.write(addr);
  for (int i = 0; i < num; i++)
  {
    Wire.write((data[i] >> 8) & 0xFF);
    Wire.write(data[i] & 0xFF);
  }
  Wire.endTransmission();
  delayMicroseconds(100);
}

uint8_t read8(uint8_t addr)
{
  uint8_t data = 0;
  Wire.beginTransmission(rom_addr);
  Wire.write(addr);
  Wire.endTransmission();
  delay(1);
  Wire.requestFrom(rom_addr, 1);
  data = Wire.read();
  return data;
}

uint16_t read16(uint16_t addr)
{
  uint16_t data = 0;
  Wire.beginTransmission(rom_addr);
  Wire.write(addr);
  Wire.endTransmission();
  delay(1);
  Wire.requestFrom(rom_addr, 2);
  data = (Wire.read() << 8) | Wire.read();
  return data;
}

void read16a(uint8_t addr, uint16_t data[], uint8_t num)
{
  Wire.beginTransmission(rom_addr);
  Wire.write(addr);
  Wire.endTransmission();
  delay(1);
  Wire.requestFrom(rom_addr, num * 2);
  for (int i = 0; i < num; i++)
  {
    data[i] = Wire.read() << 8;
    data[i] |= Wire.read();
  }
}

void save(uint8_t n)
{
  switch (n)
  {
  case 0:
    for (int i = 0; i < 5; i++)
    {
      write16a(16 + i * 8, fcolor + i * 4, 4);
      delay(3);
    }
    break;
  case 1:
    for (int i = 5; i < 10; i++)
    {
      write16a(16 + i * 8, fcolor + i * 4, 4);
      delay(3);
    }
    break;
  case 2:
    for (int i = 0; i < 5; i++)
    {
      write16a(96 + i * 8, tcolor + i * 4, 4);
      delay(3);
    }
    break;
  case 3:
    for (int i = 5; i < 10; i++)
    {
      write16a(96 + i * 8, tcolor + i * 4, 4);
      delay(3);
    }
    break;
  case 4:
    write8(5, brightness);
    delay(2);
    write8(7, glow_rate);
    delay(2);
    write8(1, 0xAA);
    delay(2);
    break;
  default:
    break;
  }
}

boolean load()
{
  if (read8(1) != 0xAA)
    return 0;
  delay(2);
  for (int i = 0; i < 10; i++)
  {
    read16a(16 + i * 8, fcolor + i * 4, 4);
    delay(3);
  }
  for (int i = 0; i < 10; i++)
  {
    read16a(96 + i * 8, tcolor + i * 4, 4);
    delay(3);
  }
  brightness = read8(5);
  delay(2);
  glow_rate = read8(7);
  delay(2);
  return 1;
}

