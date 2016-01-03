#pragma once

#include <istream>
#include <memory>
#include <vector>
#include "types.h"
#include "mbc.h"

class Cartridge
{
  std::vector<u8> rom;
  std::vector<u8> ram;
  std::unique_ptr<MemoryBankController> mbc;

  void init_mbc(uint type);
  void init_rom(uint size_code);
  void init_ram(uint size_code);

public:
  void init_cartridge(std::istream& src);

  u8 get8(uint address) const
  {
    return mbc->get8(address);
  }

  void set8(uint address, u8 value)
  {
    mbc->set8(address, value);
  }
};
