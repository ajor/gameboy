#pragma once

#include "types.h"

class LR35902;
class Memory;

class Display
{
  LR35902 &cpu;
  Memory &memory;

  struct Colour
  {
    u8 r, g, b;
  };

  static const uint width  = 160;
  static const uint height = 144;

  Colour screen_buffer[height][width];

  static const uint cycles_per_scanline = 456;

  // Numer of cycles remaining until we move on to the next scanline
  int scanline_counter = 456;

public:
  Display() = delete;
  explicit Display(LR35902 &lr35902, Memory &mem) : cpu(lr35902), memory(mem) { }

  void update(uint cycles);
};
