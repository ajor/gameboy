#pragma once

#include <stddef.h>
#include <istream>
#include <memory>
#include "types.h"
#include "mbc.h"

class Cartridge
{
  const static size_t max_rom_size = 0x400000; // 4 MB
  u8 rom[max_rom_size];
  u8 ram[0x2000]; // 8 KB

  std::unique_ptr<MemoryBankController> mbc;

public:
  Cartridge() : mbc(new MBC1(&rom[0], &ram[0])) {}

  void load_rom(std::istream& src)
  {
    src.read(reinterpret_cast<char *>(&rom), max_rom_size);
  }

  u8 get8(uint address) const
  {
    return mbc->get8(address);
  }

  void set8(uint address, u8 value)
  {
    mbc->set8(address, value);
  }
};
