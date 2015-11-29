#pragma once

#include "lr35902.h"
#include "memory.h"
#include "cartridge.h"

class Gameboy
{
  LR35902 cpu;
  Memory memory;
  Cartridge cart;

public:
  Gameboy() : cpu(memory), memory(cart) { }

  void load_rom(char *rom_file);
  void run();
  void reset();
  void set_debug(bool debug);
};
