#pragma once

#include "lr35902.h"
#include "memory.h"

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
};
