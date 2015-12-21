#pragma once

#include "lr35902.h"
#include "memory.h"
#include "cartridge.h"
#include "display.h"
#include "joypad.h"

class Gameboy
{
  LR35902 cpu;
  Memory memory;
  Cartridge cart;
  Display display;
  Joypad joypad;

public:
  Gameboy() : cpu(memory),
              memory(cart, joypad),
              display(cpu, memory),
              joypad(cpu, memory) { }

  void load_rom(char *rom_file);
  void step();
  void reset();
  void set_debug(bool debug);
  const Display::Colour *get_framebuffer() const { return display.get_framebuffer(); }
  void button_pressed(Joypad::Button::Name b) { joypad.button_pressed(b); }
  void button_released(Joypad::Button::Name b) { joypad.button_released(b); }
};
