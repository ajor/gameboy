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
public:
  Gameboy() : cpu(memory),
              memory(cart, joypad, audio),
              cart(*this),
              display(cpu, memory),
              joypad(cpu, memory),
              audio(memory) { }

  enum class DEBUG_MODE
  {
    ALL,
    CPU,
    AUDIO,
  };

  enum class GB_VERSION
  {
    ORIGINAL,
    COLOUR,
  };

  void load_rom(std::istream& rom, std::istream& ram);
  void set_save_callback(MemoryBankController::SaveRAMCallback save_ram);
  void run_to_vblank();
  void step();
  void reset();
  void set_debug(DEBUG_MODE debug_mode, bool debug);
  void set_muted(bool muted);
  void set_version(GB_VERSION version);
  const Display::Colour *get_framebuffer() const { return display.get_framebuffer(); }
  void button_pressed(Joypad::Button::Name b) { joypad.button_pressed(b); }
  void button_released(Joypad::Button::Name b) { joypad.button_released(b); }
  void save() { cart.save(); }

  bool gb_version_set = false;

private:
  LR35902 cpu;
  Memory memory;
  Cartridge cart;
  Display display;
  Joypad joypad;
  Audio audio;

  GB_VERSION gb_version;
};
