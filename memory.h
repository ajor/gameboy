#pragma once

#include <vector>
#include "types.h"

class Cartridge;

class Memory
{
  Cartridge &cart;
  std::vector<u8> vram = std::vector<u8>(0x2000);
  std::vector<u8> wram = std::vector<u8>(0x2000);
  std::vector<u8> hram = std::vector<u8>(0x7f);
  std::vector<u8> io   = std::vector<u8>(0x80);
  u8 interrupt_enable;

public:
  Memory() = delete;
  explicit Memory(Cartridge &cartridge) : cart(cartridge) { }

  void set8(uint address, u8 value)
  {
    write_byte(address, value);
  }

  u8 get8(uint address) const
  {
    return read_byte(address);
  }

  void set16(uint address, u16 value)
  {
    u8 upper = (value & 0xff00) >> 8;
    u8 lower = (value & 0x00ff);
    write_byte(address, lower);
    write_byte(address+1, upper);
  }

  u16 get16(uint address) const
  {
    u8 lower = read_byte(address);
    u8 upper = read_byte(address+1);
    u16 value = (upper << 8) | lower;
    return value;
  }

  void print(uint start, uint range=10)
  {
    for (uint i=start; i<start+range; i++)
    {
//      printf("0x%04X: 0x%02X\n", i, mem8[i]);
    }
  }

  struct IO
  {
    enum Address
    {
      DIV = 0xff04,     // Divider register (R/W)
      TIMA = 0xff05,    // Timer counter (R/W)
      TMA = 0xff06,     // Timer Modulo (R/W)
      TAC = 0xff07,     // Timer Control (R/W)

      IF = 0xff0f,      // Interrupt Flag
    };
  };

private:
  u8 read_byte(uint address) const;
  void write_byte(uint address, u8 value);
};
