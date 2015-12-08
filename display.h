#pragma once

#include "types.h"

class LR35902;
class Memory;

class Display
{
public:
  Display() = delete;
  explicit Display(LR35902 &lr35902, Memory &mem) : cpu(lr35902), memory(mem) { }

  static const uint width  = 160;
  static const uint height = 144;

  struct Colour
  {
    u8 r, g, b;
  };

  void update(uint cycles);
  const Colour *get_framebuffer() const { return &framebuffer[0][0]; }

private:
  LR35902 &cpu;
  Memory &memory;

  Colour framebuffer[height][width];

  static const uint cycles_per_scanline = 456;

  // Numer of cycles remaining until we move on to the next scanline
  int scanline_counter = 456;

  void draw_scanline();
  void draw_tiles();
  void draw_sprites();
};
