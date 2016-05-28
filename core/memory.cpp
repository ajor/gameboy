#include "memory.h"
#include "cartridge.h"
#include "joypad.h"
#include "audio.h"

u8 Memory::read_byte(uint address) const
{
  if (address < 0x8000 || (address >= 0xa000 && address < 0xc000))
  {
    // ROM / external RAM
    return cart.get8(address);
  }
  else if (address >= 0x8000 && address < 0xa000)
  {
    // VRAM - Video RAM
    return vram.at(active_vram_bank*0x2000 + address - 0x8000);
  }
  else if (address >= 0xc000 && address < 0xd000)
  {
    // WRAM - Work RAM bank 0
    return wram.at(address - 0xc000);
  }
  else if (address >= 0xd000 && address < 0xe000)
  {
    // Switchable Work RAM bank (1 - 7)
    return wram.at(active_wram_bank*0x1000 + address - 0xd000);
  }
  else if (address >= 0xe000 && address < 0xfe00)
  {
    // ECHO - Mirror of C000 - DDFF
    return wram.at(address - 0xe000);
  }
  else if (address >= 0xfe00 && address < 0xfea0)
  {
    // Sprite attribute table
    return oam.at(address - 0xfe00);
  }
  else if (address >= 0xfea0 && address < 0xff00)
  {
    // Not usable
    return 0;
  }
  else if (address >= 0xff00 && address < 0xff80)
  {
    // IO registers
    if (address >= IO::NR10 && address <= IO::WAVE + 0xf)
    {
      return audio.read_byte(address);
    }
    else if (address == IO::BGPD)
    {
      return cgb_background_palettes[cgb_background_palette_index];
    }
    else if (address == IO::OBPD)
    {
      return cgb_sprite_palettes[cgb_sprite_palette_index];
    }

    return io.at(address - 0xff00);
  }
  else if (address >= 0xff80 && address < 0xffff)
  {
    // HRAM - High RAM
    return hram.at(address - 0xff80);
  }
  else if (address == 0xffff)
  {
    // Interrupt enable register
    return interrupt_enable;
  }

  // Should never get here
  abort();
}

void Memory::write_byte(uint address, u8 value)
{
  if (address < 0x8000 || (address >= 0xa000 && address < 0xc000))
  {
    // ROM / external RAM
    cart.set8(address, value);
  }
  else if (address >= 0x8000 && address < 0xa000)
  {
    // VRAM - Video RAM
    vram.at(address - 0x8000) = value;
  }
  else if (address >= 0xc000 && address < 0xd000)
  {
    // WRAM - Work RAM bank 0
    wram.at(address - 0xc000) = value;
  }
  else if (address >= 0xd000 && address < 0xe000)
  {
    // Switchable Work RAM bank (1 - 7)
    // TODO make switchable
    wram.at(address - 0xc000) = value;
  }
  else if (address >= 0xe000 && address < 0xfe00)
  {
    // ECHO - Mirror of C000 - DDFF
    wram.at(address - 0xe000) = value;
  }
  else if (address >= 0xfe00 && address < 0xfea0)
  {
    // Sprite attribute table
    oam.at(address - 0xfe00) = value;
  }
  else if (address >= 0xfea0 && address < 0xff00)
  {
    // Not usable
    return;
  }
  else if (address >= 0xff00 && address < 0xff80)
  {
    // IO registers
    if (address == IO::DIV || address == IO::LY)
    {
      value = 0;
    }
    else if (address == IO::DMA)
    {
      dma_transfer(value);
    }
    else if (address == IO::JOYP)
    {
      // First set the control bits, then update the joypad state
      io.at(IO::JOYP - 0xff00) = value;
      value = joypad.get_button_state();
    }
    else if (address >= IO::NR10 && address <= IO::WAVE + 0xf)
    {
      audio.write_byte(address, value);
    }
    else if (address == IO::VBK)
    {
      active_vram_bank = value & 0x1;
    }
    else if (address == IO::SVBK)
    {
      active_wram_bank = value & 0x7;
    }
    else if (address == IO::BGPD)
    {
      cgb_background_palettes[cgb_background_palette_index] = value;
      if (cgb_background_palette_autoinc)
        cgb_background_palette_index++;
    }
    else if (address == IO::OBPD)
    {
      cgb_sprite_palettes[cgb_sprite_palette_index] = value;
      if (cgb_sprite_palette_autoinc)
        cgb_sprite_palette_index++;
    }
    else if (address == IO::BGPI)
    {
      cgb_background_palette_index = value & 0x3f;
      cgb_background_palette_autoinc = (value >> 7) & 0x1;
    }
    else if (address == IO::OBPI)
    {
      cgb_sprite_palette_index = value & 0x3f;
      cgb_sprite_palette_autoinc = (value >> 7) & 0x1;
    }

    io.at(address - 0xff00) = value;
  }
  else if (address >= 0xff80 && address < 0xffff)
  {
    // HRAM - High RAM
    hram.at(address - 0xff80) = value;
  }
  else if (address == 0xffff)
  {
    // Interrupt enable register
    interrupt_enable = value;
  }
  else
  {
    // Should never get here
    abort();
  }
}

void Memory::direct_io_write8(uint address, u8 value)
{
  io.at(address - 0xff00) = value;
}

void Memory::dma_transfer(uint address)
{
  address <<= 8;
  for (uint i=0; i<0xa0; i++)
  {
    set8(0xfe00+i, get8(address+i));
  }
}
