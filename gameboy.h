#pragma once

#include "lr35902.h"
#include "memory.h"
#include "cartridge.h"
#include "display.h"

class Gameboy
{
  LR35902 cpu;
  Memory memory;
  Cartridge cart;
  Display display;

public:
  Gameboy() : cpu(memory), memory(cart), display(cpu, memory) { }

  void load_rom(char *rom_file);
  void step();
  void reset();
  void set_debug(bool debug);
  const Display::Colour *get_framebuffer() const { return display.get_framebuffer(); }
};
