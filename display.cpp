#include "display.h"
#include "lr35902.h"
#include "memory.h"

void Display::update(uint cycles)
{
  scanline_counter -= cycles;

  if (scanline_counter <= 0)
  {
    scanline_counter += cycles_per_scanline;

    u8 scanline = memory.get8(Memory::IO::LY) + 1;
    if (scanline > 153)
    {
      scanline = 0;
    }
    memory.direct_io_write8(Memory::IO::LY, scanline);

    if (scanline < 144)
    {
      draw_scanline();
    }
    else if (scanline == 144)
    {
      cpu.raise_interrupt(LR35902::Interrupt::VBLANK);
    }
  }
}

void Display::draw_scanline()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);

  if (LCDC & (1<<0))
  {
    draw_tiles();
  }
  if (LCDC & (1<<1))
  {
    draw_sprites();
  }
}

void Display::draw_tiles()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);
  u8 LY   = memory.get8(Memory::IO::LY);

  uint base_tile_map_addr;
  if (LCDC & (1<<6))
  {
    // 0x9C00 - 9FFF
    base_tile_map_addr = 0x9C00;
  }
  else
  {
    // 0x9800 - 9BFF
    base_tile_map_addr = 0x9800;
  }

  uint base_tile_data_addr;
  bool signed_pattern_numbers;
  if (LCDC & (1<<4))
  {
    // 0x8000 - 0x8FFF
    // Offsets will be unsigned
    base_tile_data_addr = 0x8000;
    signed_pattern_numbers = false;
  }
  else
  {
    // 0x8800 - 0x97FF
    // Offsets will be signed, set addr to middle of range
    base_tile_data_addr = 0x9000;
    signed_pattern_numbers = true;
  }

  u8 SCY = memory.get8(Memory::IO::SCY);
  u8 SCX = memory.get8(Memory::IO::SCX);

  uint background_y = (SCY + LY)%256;
  uint tile_row = background_y/8;
  uint tile_y = background_y%8;

  for (uint screenx=0; screenx<width; screenx++)
  {
    uint background_x = (SCX + screenx)%256;
    uint tile_col = background_x/8;
    uint tile_x = background_x%8;

    uint tile_map_addr = base_tile_map_addr + tile_row*32 + tile_col;
    u8 tile_num = memory.get8(tile_map_addr);

    uint tile_data_addr;
    if (signed_pattern_numbers)
    {
      tile_data_addr = base_tile_data_addr + ((s8)tile_num)*16;
    }
    else
    {
      tile_data_addr = base_tile_data_addr + ((u8)tile_num)*16;
    }

    // TODO select the correct data from the tile
    uint tile_byte = tile_y*2 + tile_x/4;
    u8 pixel = memory.get8(tile_data_addr + tile_byte);

    framebuffer[LY][screenx] = {pixel, 0, 0};
  }
}

void Display::draw_sprites()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);
}
