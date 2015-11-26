#pragma once

#include <istream>
#include <memory>
#include <vector>
#include "types.h"
#include "mbc.h"

class Cartridge
{
  const static size_t max_rom_size = 0x400000; // 4 MB
  std::vector<u8> rom;
  std::vector<u8> ram;
  std::unique_ptr<MemoryBankController> mbc;

public:
  Cartridge() : rom(max_rom_size), ram(0x2000), mbc(new MBC1(rom, ram)) {}

  void load_rom(std::istream& src)
  {
    src.read(reinterpret_cast<char *>(rom.data()), max_rom_size);
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
