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

    if (scanline == 144)
    {
      draw();
      cpu.raise_interrupt(LR35902::Interrupt::VBLANK);
    }
  }
}

void Display::draw()
{
  for (uint y=0; y<height; y++)
  {
    for (uint x=0; x<width; x++)
    {
      screen_buffer[y][x] = {0xff, 0, 0};
    }
  }
}
