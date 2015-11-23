#pragma once

#include <stddef.h>
#include <istream>
#include "types.h"

class Cartridge
{
  const static size_t max_rom_size = 0x400000; // 4 MB
  u8 rom[max_rom_size];
  uint curr_mem_block_addr = 0x4000; // 4 KB

public:
  void load_rom(std::istream& src)
  {
    src.read(reinterpret_cast<char *>(&rom), max_rom_size);
  }

  const u8 *get_memory_block_0() const
  {
    return &rom[0];
  }

  const u8 *get_current_memory_block() const
  {
    return &rom[curr_mem_block_addr];
  }

  void set_current_memory_block(uint n)
  {
    curr_mem_block_addr = 0x4000 * n;
  }
};
