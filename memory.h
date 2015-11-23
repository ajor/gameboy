#pragma once

#include "types.h"
#include "cartridge.h"

class Memory
{
  Cartridge &cart;

public:
  Memory() = delete;
  explicit Memory(Cartridge &cartridge) : cart(cartridge) { }

  void set8(unsigned int address, uint8_t value)
  {
    write_byte(address, value);
  }

  uint8_t get8(unsigned int address)
  {
    return read_byte(address);
  }

  void set16(unsigned int address, uint16_t value)
  {
    uint8_t upper = (value & 0xff00) >> 8;
    uint8_t lower = (value & 0x00ff);
    write_byte(address, lower);
    write_byte(address+1, upper);
  }

  uint16_t get16(unsigned int address)
  {
    uint8_t lower = read_byte(address);
    uint8_t upper = read_byte(address+1);
    uint16_t value = (upper << 8) | lower;
    return value;
  }

  void print(unsigned int start, unsigned int range=10)
  {
    for (unsigned int i=start; i<start+range; i++)
    {
//      printf("0x%04X: 0x%02X\n", i, mem8[i]);
    }
  }

private:
  u8 read_byte(uint address);
  void write_byte(uint address, u8 value);
};
