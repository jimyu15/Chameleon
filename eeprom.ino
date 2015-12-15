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
  delay(1);
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
  Wire.requestFrom(rom_addr, 1);
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

void save()
{
  for (int i = 0; i < 5; i++)
    write16a(10 + i * 20, fcolor + i * 10, 10);
  for (int i = 0; i < 5; i++)
    write16a(110 + i * 20, tcolor + i * 10, 10);
  
  write8(5, brightness);
  write8(7, glow_rate);
  write8(1, 0xAA);
}

boolean load()
{
  if (read8(1) != 0xAA)
    return 0;
  for (int i = 0; i < 5; i++)
    read16a(10 + i * 20, fcolor + i * 10, 10);
  for (int i = 0; i < 5; i++)
    read16a(110 + i * 20, tcolor + i * 10, 10);
  
  brightness = read8(5);
  glow_rate = read8(7);
  return 1;
}

