#include "display.h"
#include "lr35902.h"
#include "memory.h"

void Display::update(uint cycles)
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);

  if (LCDC & (1<<7)) // LCD enabled?
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

    update_status(); // put at beginning?
  }
}

void Display::draw_scanline()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);

  if (LCDC & (1<<0))
  {
    draw_background();
  }
  if (LCDC & (1<<5))
  {
    draw_window();
  }
  if (LCDC & (1<<1))
  {
    draw_sprites();
  }
}

void Display::draw_background()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);
  u8 LY   = memory.get8(Memory::IO::LY);

  uint base_tile_map_addr;
  if (LCDC & (1<<3))
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

    framebuffer[LY][screenx] = display_palette[colour];
  }
}

void Display::draw_window()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);
  u8 LY   = memory.get8(Memory::IO::LY);
  u8 WY   = memory.get8(Memory::IO::WY);
  u8 WX   = memory.get8(Memory::IO::WX);

  if (WY > LY || WX >= 167)
  {
    // Window isn't visible / not drawn on this scanline
    return;
  }

  uint base_window_map_addr;
  if (LCDC & (1<<6))
  {
    // 0x9C00 - 9FFF
    base_window_map_addr = 0x9C00;
  }
  else
  {
    // 0x9800 - 9BFF
    base_window_map_addr = 0x9800;
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

  uint window_y = (LY - WY)%256;
  uint tile_row = window_y/8;
  uint tile_y = window_y%8;

  // Set WX to left side of window
  WX -= 7;

  for (uint screenx=0; screenx<width; screenx++)
  {
    uint window_x = (screenx - WX)%256;
    uint tile_col = window_x/8;
    uint tile_x = window_x%8;

    // Tile map is 32x32 tiles, with 1 byte per tile
    uint tile_map_addr = base_window_map_addr + tile_row*32 + tile_col;
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

    framebuffer[LY][screenx] = display_palette[colour];
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

    u8 y_pos = memory.get8(sprite_attr_addr);     // sprite top y coordinate + 16
    u8 x_pos = memory.get8(sprite_attr_addr + 1); // sprite left x coordinate + 8
    u8 pattern_number = memory.get8(sprite_attr_addr + 2);
    u8 flags = memory.get8(sprite_attr_addr + 3);

    if (y_pos > LY + 16 || y_pos + sprite_height <= (uint)(LY + 16))
    {
      // This sprite doesn't appear on the current scanline
      continue;
    }

    if (use_8x16_sprites)
    {
      pattern_number &= 0xfe; // Set LSB to 0
    }

    bool low_priority = (flags >> 7) & 0x1;
    bool flip_y = (flags >> 6) & 0x1;
    bool flip_x = (flags >> 5) & 0x1;
    uint palette_num = (flags >> 4) & 0x1;

    // Sprites are either 8x8 or 8x16 pixels, with eiter 16 or 32 bytes each
    uint sprite_size = 2 * sprite_height;
    uint sprite_data_addr = base_sprite_data_addr + pattern_number*sprite_size;

    uint sprite_y = LY - y_pos + 16;
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

      if (colour_id == 0) // Colour 0 is transparent
      {
        continue;
      }

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

      uint screenx = (x_pos-8+sprite_x)%256;
      if (screenx >= width)
      {
        // Pixel is off screen
        continue;
      }

      // Low priority sprites are only drawn on colour 0 backgrounds
      u8 BGP = memory.get8(Memory::IO::BGP);
      u8 colour_0 = (BGP & 0x3);
      if (!low_priority ||
          (framebuffer[LY][screenx].r == display_palette[colour_0].r &&
           framebuffer[LY][screenx].b == display_palette[colour_0].b &&
           framebuffer[LY][screenx].g == display_palette[colour_0].g))
      {
        framebuffer[LY][screenx] = display_palette[colour];
      }
    }
  }
}

void Display::update_status()
{
  u8 STAT = memory.get8(Memory::IO::STAT);

  u8 scanline = memory.get8(Memory::IO::LY);
  MODE::Mode mode;

  if (scanline > 144)
  {
    mode = MODE::VBLANK;
  }
  else if (scanline_counter > cycles_per_scanline - 80)
  {
    mode = MODE::OAM;
  }
  else if (scanline_counter > cycles_per_scanline - 80 - 172)
  {
    mode = MODE::VRAM;
  }
  else
  {
    mode = MODE::HBLANK;
  }

  const MODE::Mode current_mode = static_cast<MODE::Mode>(STAT & 0x3);
  if (current_mode != mode &&
      mode != MODE::VRAM &&
      (STAT >> (mode + 3)) & 0x1)
  {
    cpu.raise_interrupt(LR35902::Interrupt::LCD);
  }

  // Update mode
  STAT &= ~0x3;
  STAT |= mode;

  const u8 LYC = memory.get8(Memory::IO::LYC);
  if (LYC == scanline)
  {
    // Set coincidence flag
    STAT |= (1<<2);
    if ((STAT >> 6) & 0x1)
    {
      cpu.raise_interrupt(LR35902::Interrupt::LCD);
    }
  }
  else
  {
    // Clear coincidence flag
    STAT &= ~(1<<2);
  }

  memory.set8(Memory::IO::STAT, STAT);
}
