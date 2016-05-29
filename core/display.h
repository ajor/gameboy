#pragma once

#include <vector>
#include "types.h"

class LR35902;
class Memory;

class Display
{
public:
  enum class GB_VERSION
  {
    ORIGINAL,
    COLOUR,
  } gb_version;

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

  u8 read_byte(uint address) const;
  void write_byte(uint address, u8 value);

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

  // Gameboy Colour palettes
  std::vector<u8> cgb_background_palettes = std::vector<u8>(0x40);
  std::vector<u8> cgb_sprite_palettes = std::vector<u8>(0x40);
  int cgb_background_palette_index, cgb_sprite_palette_index;
  bool cgb_background_palette_autoinc, cgb_sprite_palette_autoinc;

  void draw_scanline();
  void draw_background();
  void clear_background();
  void draw_window();
  void draw_sprites();

  void update_status();
};
