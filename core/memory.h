#pragma once

#include <vector>
#include "types.h"

class Cartridge;
class Joypad;
class Audio;

class Memory
{
public:
  Memory() = delete;
  explicit Memory(Cartridge &cartridge, Joypad &j, Audio &a) : cart(cartridge),
                                                               joypad(j),
                                                               audio(a) { }

  void set8(uint address, u8 value)
  {
    write_byte(address, value);
  }

  u8 get8(uint address) const
  {
    return read_byte(address);
  }

  void set16(uint address, u16 value)
  {
    u8 upper = (value & 0xff00) >> 8;
    u8 lower = (value & 0x00ff);
    write_byte(address, lower);
    write_byte(address+1, upper);
  }

  u16 get16(uint address) const
  {
    u8 lower = read_byte(address);
    u8 upper = read_byte(address+1);
    u16 value = (upper << 8) | lower;
    return value;
  }

  // Used to directly set the memory at a given address
  // without value being manipulated first
  void direct_io_write8(uint address, u8 value);

  void print(uint start, uint range=10)
  {
    for (uint i=start; i<start+range; i++)
    {
//      printf("0x%04X: 0x%02X\n", i, mem8[i]);
    }
  }

  struct IO
  {
    enum Address
    {
      // Joypad
      JOYP = 0xff00,    // Joypad

      // Timer
      DIV  = 0xff04,    // Divider register
      TIMA = 0xff05,    // Timer counter
      TMA  = 0xff06,    // Timer Modulo
      TAC  = 0xff07,    // Timer Control

      // Audio
      //   Channel 1 - tone & sweep
      NR10 = 0xff10,    // Channel 1 sweep
      NR11 = 0xff11,    // Channel 1 sound length/wave pattern duty
      NR12 = 0xff12,    // Channel 1 volume envelope
      NR13 = 0xff13,    // Channel 1 frequency low
      NR14 = 0xff14,    // Channel 1 frequency high

      //   Channel 2 - tone
      NR21 = 0xff16,    // Channel 2 sound length/wave pattern duty
      NR22 = 0xff17,    // Channel 2 volume envelope
      NR23 = 0xff18,    // Channel 2 frequency low
      NR24 = 0xff19,    // Channel 2 frequency high

      //   Channel 3 - wave output
      NR30 = 0xff1a,    // Channel 3 sound on/off
      NR31 = 0xff1b,    // Channel 3 sound length
      NR32 = 0xff1c,    // Channel 3 select output level
      NR33 = 0xff1d,    // Channel 3 frequency low
      NR34 = 0xff1e,    // Channel 3 frequency high

      //   Channel 4 - noise
      NR41 = 0xff20,    // Channel 4 sound length
      NR42 = 0xff21,    // Channel 4 volume envelope
      NR43 = 0xff22,    // Channel 4 polynomial counter
      NR44 = 0xff23,    // Channel 4 counter/consecutive; initial

      //   Sound control registers
      NR50 = 0xff24,    // Channel control / on-off / volume
      NR51 = 0xff25,    // Sound output terminal selection
      NR52 = 0xff26,    // Sound on/off

      WAVE = 0xff30,    // Wave pattern RAM, 16 bytes

      // Display
      LCDC = 0xff40,    // LCD Control
      STAT = 0xff41,    // LCDC Status
      SCY  = 0xff42,    // Scroll Y
      SCX  = 0xff43,    // Scroll X
      LY   = 0xff44,    // LCDC Y coordinate (scanline)
      LYC  = 0xff45,    // LY Compare
      DMA  = 0xff46,    // Direct Memory Access
      BGP  = 0xff47,    // Background & Window Palette Data (non GBC)
      OBP0 = 0xff48,    // Object Palette 0 Data (non GBC)
      OBP1 = 0xff49,    // Object Palette 1 Data (non GBC)
      WY   = 0xff4a,    // Window Y position
      WX   = 0xff4b,    // Window X position

      // Gameboy Colour Registers
      KEY1 = 0xff4d,    // Prepare speed switch
      RP   = 0xff56,    // Infrared communications port
      SVBK = 0xff70,    // WRAM bank

      // DMA Transfers - Gameboy Colour only
      HDMA1 = 0xff51,   // New DMA source, high
      HDMA2 = 0xff52,   // New DMA source, low
      HDMA3 = 0xff53,   // New DMA destination, high
      HDMA4 = 0xff54,   // New DMA destination, low
      HDMA5 = 0xff55,   // New DMA length/mode/start

      // Colour Palettes - Gameboy Colour only
      BGPI = 0xff68,    // BCPS/BGPI Background Palette Index
      BGPD = 0xff69,    // BCPD/BGPD Background Palette Data
      OBPI = 0xff6a,    // OCPS/OBPI Sprite Palette Index
      OBPD = 0xff6b,    // OCPD/OBPD Sprite Palette Data

      // Interrupt
      IF = 0xff0f,      // Interrupt Flag
      IE = 0xffff,      // Interrupt Enable
    };
  };

private:
  u8 read_byte(uint address) const;
  void write_byte(uint address, u8 value);
  void dma_transfer(uint address);

  Cartridge &cart;
  Joypad &joypad;
  Audio &audio;

  std::vector<u8> vram = std::vector<u8>(0x2000);
  std::vector<u8> wram = std::vector<u8>(0x8000);
  std::vector<u8> hram = std::vector<u8>(0x7f);
  std::vector<u8> oam  = std::vector<u8>(0xa0);
  std::vector<u8> io   = std::vector<u8>(0x80);
  u8 interrupt_enable;

  uint active_wram_bank = 1;
};
