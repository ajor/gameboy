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

    // Tile map is 32x32 tiles, with 1 byte per tile
    uint tile_map_addr = base_tile_map_addr + tile_row*32 + tile_col;
    u8 tile_num = memory.get8(tile_map_addr);

    // Tile data is 32x32 tiles, with 16 bytes per tile
    uint tile_data_addr;
    if (signed_pattern_numbers)
    {
      tile_data_addr = base_tile_data_addr + ((s8)tile_num)*16;
    }
    else
    {
      tile_data_addr = base_tile_data_addr + ((u8)tile_num)*16;
    }

    // Tiles are 8x8 pixels, with 2 bits per pixel
    // i.e 2 bytes per line and 4 pixels per byte
    //   The 2 bits per pixel aren't adjacent, they are in the
    //   same position in each of the 2 bytes for their line.
    uint tile_byte_offset = tile_y*2;
    u8 tile_byte1 = memory.get16(tile_data_addr + tile_byte_offset);
    u8 tile_byte2 = memory.get16(tile_data_addr + tile_byte_offset + 1);

    uint bit = 7 - tile_x;
    uint colour_id = ((tile_byte1 >> bit) & 0x1) |
                     ((tile_byte2 >> bit) & 0x1) << 1;

    // Get the colour from the background palette register
    // 0 = white, 3 = black
    u8 BGP = memory.get8(Memory::IO::BGP);
    u8 colour = (BGP >> (colour_id * 2)) & 0x3;
    colour *= 0xff/3;
    colour = 0xff - colour;

    framebuffer[LY][screenx] = {colour, colour, colour};
  }
}

void Display::draw_sprites()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);
  u8 LY   = memory.get8(Memory::IO::LY);

  const uint base_sprite_data_addr = 0x8000;
  const uint base_sprite_attr_addr = 0xfe00;

  bool use_8x16_sprites = (LCDC >> 2) & 0x1;
  uint sprite_height = use_8x16_sprites ? 16 : 8;

  // Go through all the sprites in reverse order
  // so sprite 0 is drawn on top of sprite 39 etc.
  for (int i=39; i>=0; i--)
  {
    // Sprite attribute blocks are 4 bytes each
    uint sprite_attr_addr = base_sprite_attr_addr + i*4;

    u8 y_pos = memory.get8(sprite_attr_addr);
    u8 x_pos = memory.get8(sprite_attr_addr + 1);
    u8 pattern_number = memory.get8(sprite_attr_addr + 2);
    u8 flags = memory.get8(sprite_attr_addr + 3);

    if (!(y_pos <= LY && y_pos + sprite_height > LY))
    {
      // This sprite doesn't appear on the current scanline
      continue;
    }

    if (use_8x16_sprites)
    {
      pattern_number &= 0xfe; // Set LSB to 0
    }

    bool high_priority = (flags >> 7) & 0x1;
    bool flip_y = (flags >> 6) & 0x1;
    bool flip_x = (flags >> 5) & 0x1;
    uint palette_num = (flags >> 4) & 0x1;

    // Sprites are either 8x8 or 8x16 pixels, with eiter 16 or 32 bytes each
    uint sprite_size = 2 * sprite_height;
    uint sprite_data_addr = base_sprite_data_addr + pattern_number*sprite_size;

    uint sprite_y = LY - y_pos;
    if (flip_y)
    {
      sprite_y = sprite_height - sprite_y;
    }

    // Get the data for this line of the sprite
    uint sprite_byte_offset = sprite_y*2;
    u8 sprite_byte1 = memory.get16(sprite_data_addr + sprite_byte_offset);
    u8 sprite_byte2 = memory.get16(sprite_data_addr + sprite_byte_offset + 1);

    for (uint sprite_x=0; sprite_x<8; sprite_x++)
    {
      uint bit = flip_x ? sprite_x : 7 - sprite_x;
      uint colour_id = ((sprite_byte1 >> bit) & 0x1) |
                       ((sprite_byte2 >> bit) & 0x1) << 1;

      // Get the colour from the background palette register
      // 0 = white, 3 = black
      u8 palette;
      if (palette_num == 0)
      {
        palette = memory.get8(Memory::IO::OBP0);
      }
      else
      {
        palette = memory.get8(Memory::IO::OBP1);
      }
      u8 colour = (palette >> (colour_id * 2)) & 0x3;
      colour *= 0xff/3;
      colour = 0xff - colour;

      if (x_pos != 0)
      {
        x_pos++;
        x_pos--;
      }
      framebuffer[LY][x_pos+sprite_x] = {colour, colour, colour};
    }
  }
}
