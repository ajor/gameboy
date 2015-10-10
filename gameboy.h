#pragma once

#include "LR35902.h"
#include "memory.h"

class Gameboy
{
  LR35902 cpu;
  Memory<8192> memory;

public:
  Gameboy() : cpu(memory) { }

  void load_rom(char *rom_file);
  void run();
  void reset();
};
