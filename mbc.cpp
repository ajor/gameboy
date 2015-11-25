#include "mbc.h"

u8 MBC1::get8(uint address) const
{
  if (address < 0x4000)
  {
    // ROM bank 0
    return rom[address];
  }
  else if (address >= 0x4000 && address < 0x8000)
  {
    // Switchable ROM bank
    return rom[active_rom_bank_addr + address - 0x4000];
  }
}

void MBC1::set8(uint address, u8 value)
{
}
