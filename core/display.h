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

  Colour display_palette[4] = {
                               {0xff, 0xff, 0xff},
                               {0xaa, 0xaa, 0xaa},
                               {0x55, 0x55, 0x55},
                               {0x00, 0x00, 0x00},
                              };

  void update(uint cycles);
  const Colour *get_framebuffer() const { return &framebuffer[0][0]; }
  bool in_vblank() { return vblank; }

private:
  LR35902 &cpu;
  Memory &memory;

  Colour framebuffer[height][width];

  static const int cycles_per_scanline = 456;

  // Numer of cycles remaining until we move on to the next scanline
  int scanline_counter = 456;

  bool vblank = false;

  struct MODE
  {
    enum Mode
    {
      HBLANK = 0,
      VBLANK = 1,
      OAM    = 2,
      VRAM   = 3,
    };
  };

  void draw_scanline();
  void draw_background();
  void draw_window();
  void draw_sprites();

  void update_status();
};
