#include "mbc.h"

u8 NoMBC::get8(uint address) const
{
  if (address < 0x8000)
  {
    // ROM
    return rom.at(address);
  }
  else if (address >= 0xa000 && address < 0xc000)
  {
    // RAM
    return ram.at(address - 0xa000);
  }

  // Should never get here
  return 0;
}

void NoMBC::set8(uint address, u8 value)
{
  if (address >= 0xa000 && address < 0xc000)
  {
    // RAM
    ram.at(address - 0xa000) = value;
  }
}

u8 MBC1::get8(uint address) const
{
  if (address < 0x4000)
  {
    // ROM bank 0
    return rom.at(address);
  }
  else if (address >= 0x4000 && address < 0x8000)
  {
    // Switchable ROM bank (1 - 127)
    return rom.at(active_rom_bank*0x4000 + address - 0x4000);
  }
  else if (address >= 0xa000 && address < 0xc000)
  {
    // Switchable RAM bank (0 - 3)
    if (ram_enabled)
      return ram.at(active_ram_bank*0x2000 + address - 0xa000);
    else
      return 0;
  }

  // Should never get here
  return 0;
}

void MBC1::set8(uint address, u8 value)
{
  if (address < 0x2000)
  {
    // RAM enable
    if ((value & 0xa) == 0xa)
      ram_enabled = true;
    else
      ram_enabled = false;
  }
  else if (address >= 0x2000 && address < 0x4000)
  {
    // Select ROM bank (lower 5 bits)
    value &= 0x1f;
    if (value == 0)
      value = 1;
    active_rom_bank = (active_rom_bank & 0x60) | value;
  }
  else if (address >= 0x4000 && address < 0x6000)
  {
    // Select RAM bank / select ROM bank (upper 2 bits)
    value &= 3;
    if (banking_mode == BankingMode::RAM)
    {
      active_ram_bank = value;
    }
    else
    {
      active_rom_bank = (value << 5) | (active_rom_bank & 0x1f);
    }
  }
  else if (address >= 0x6000 && address < 0x8000)
  {
    // Select ROM/RAM mode
    if (value)
      banking_mode = BankingMode::RAM;
    else
      banking_mode = BankingMode::ROM;
  }
  else if (address >= 0xa000 && address < 0xc000)
  {
    // Switchable RAM bank (0 - 3)
    ram.at(active_ram_bank*0x2000 + address - 0xa000) = value;
  }
}

u8 MBC3::get8(uint address) const
{
  if (address < 0x4000)
  {
    // ROM bank 0
    return rom.at(address);
  }
  else if (address >= 0x4000 && address < 0x8000)
  {
    // Switchable ROM bank (1 - 127)
    return rom.at(active_rom_bank*0x4000 + address - 0x4000);
  }
  else if (address >= 0xa000 && address < 0xc000)
  {
    if (ram_enabled)
    {
      if (banking_mode == BankingMode::RAM)
      {
        // Switchable RAM bank (0 - 3)
        return ram.at(active_ram_bank*0x2000 + address - 0xa000);
      }
      else
      {
        // RTC register
        return rtc[active_rtc];
      }
    }
    else
    {
      return 0;
    }
  }

  // Should never get here
  return 0;
}

void MBC3::set8(uint address, u8 value)
{
  if (address < 0x2000)
  {
    // RAM (and timer) enable
    if ((value & 0xa) == 0xa)
      ram_enabled = true;
    else
      ram_enabled = false;
  }
  else if (address >= 0x2000 && address < 0x4000)
  {
    // Select ROM bank
    if (value == 0)
      value = 1;
    active_rom_bank = value;
  }
  else if (address >= 0x4000 && address < 0x6000)
  {
    // Select RAM bank / select RTC register
    if (value <= 0x3)
    {
      banking_mode = BankingMode::RAM;
      active_ram_bank = value;
    }
    else if (value >= 0x8 && value <= 0xc)
    {
      banking_mode = BankingMode::RTC;
      active_rtc = value - 0x8;
    }
  }
  else if (address >= 0x6000 && address < 0x8000)
  {
    // Latch clock data
    // TODO
  }
  else if (address >= 0xa000 && address < 0xc000)
  {
    if (ram_enabled)
    {
      if (banking_mode == BankingMode::RAM)
      {
        // Switchable RAM bank (0 - 3)
        ram.at(active_ram_bank*0x2000 + address - 0xa000) = value;
      }
      else
      {
        // RTC register
        rtc[active_rtc] = value;
      }
    }
  }
}
