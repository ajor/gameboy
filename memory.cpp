#include "memory.h"
#include "cartridge.h"
#include <assert.h>

u8 Memory::read_byte(uint address)
{
  if (address < 0x4000)
  {
    // ROM bank 0
    return cart.get_memory_block_0()[address];
  }
  else if (address < 0x8000)
  {
    // Switchable ROM bank (1 - 255)
    return cart.get_current_memory_block()[address-0x4000];
  }
  else if (address < 0xa000)
  {
    // VRAM - Video RAM
  }
  else if (address < 0xc000)
  {
    // External RAM
  }
  else if (address < 0xd000)
  {
    // WRAM - Work RAM bank 0
  }
  else if (address < 0xe000)
  {
    // Switchable Work RAM bank (1 - 7)
  }
  else if (address < 0xfe00)
  {
    // ECHO - Mirror of C000 - DDFF
  }
  else if (address < 0xfea0)
  {
    // Sprite attribute table
  }
  else if (address < 0xff00)
  {
    // Not usable
  }
  else if (address < 0xff80)
  {
    // IO registers
  }
  else if (address < 0xffff)
  {
    // HRAM - High RAM
  }
  else if (address == 0xffff)
  {
    // Interrupt enable register
  }

  // Should never get here
  assert(false);
  return 0;
}

void Memory::write_byte(uint address, u8 value)
{
}
