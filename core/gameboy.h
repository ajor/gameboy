#pragma once

#include <istream>

#include "lr35902.h"
#include "memory.h"
#include "cartridge.h"
#include "display.h"
#include "joypad.h"
#include "audio.h"

class Gameboy
{
  LR35902 cpu;
  Memory memory;
  Cartridge cart;
  Display display;
  Joypad joypad;
  Audio audio;

public:
  Gameboy() : cpu(memory),
              memory(cart, joypad, audio),
              cart(*this),
              display(cpu, memory),
              joypad(cpu, memory),
              audio(memory) { }

  enum DEBUG_MODE
  {
    DEBUG_MODE_ALL,
    DEBUG_MODE_CPU,
    DEBUG_MODE_AUDIO,
  };

  enum GB_VERSION
  {
    GB_VERSION_UNDEFINED,
    GB_VERSION_ORIGINAL,
    GB_VERSION_COLOUR,
  };

  GB_VERSION gb_version = GB_VERSION_UNDEFINED;

  void load_rom(std::istream& rom, std::istream& ram);
  void set_save_callback(MemoryBankController::SaveRAMCallback save_ram);
  void run_to_vblank();
  void step();
  void reset();
  void set_debug(DEBUG_MODE debug_mode, bool debug);
  void set_muted(bool muted);
  const Display::Colour *get_framebuffer() const { return display.get_framebuffer(); }
  void button_pressed(Joypad::Button::Name b) { joypad.button_pressed(b); }
  void button_released(Joypad::Button::Name b) { joypad.button_released(b); }
  void save() { cart.save(); }
};
