#include "joypad.h"
#include "memory.h"
#include "lr35902.h"
#include <stdio.h>

Joypad::Joypad(LR35902 &lr35902, Memory &mem) : cpu(lr35902), memory(mem)
{
  // No buttons pressed at start
  memory.direct_io_write8(Memory::IO::JOYP, 0xff);
}

void Joypad::button_pressed(Button::Name b)
{
  buttons[b] = true;
  memory.direct_io_write8(Memory::IO::JOYP, get_button_state());
}

void Joypad::button_released(Button::Name b)
{
  buttons[b] = false;
  memory.direct_io_write8(Memory::IO::JOYP, get_button_state());
}

u8 Joypad::get_button_state() const
{
  u8 JOYP = memory.get8(Memory::IO::JOYP);

  bool direction_keys = !(JOYP & (1 << 4));
  bool button_keys = !(JOYP & (1 << 5));

  // Right / Button A
  bool bit0 = (direction_keys && buttons[Button::RIGHT]) ||
              (button_keys && buttons[Button::A]);

  // Left / Button B
  bool bit1 = (direction_keys && buttons[Button::LEFT]) ||
              (button_keys && buttons[Button::B]);

  // Up / Select
  bool bit2 = (direction_keys && buttons[Button::UP]) ||
              (button_keys && buttons[Button::SELECT]);

  // Down / Start
  bool bit3 = (direction_keys && buttons[Button::DOWN]) ||
              (button_keys && buttons[Button::START]);

  JOYP = (button_keys << 5) | (direction_keys << 4) |
         (bit3 << 3) | (bit2 << 2) | (bit1 << 1) | bit0;

  if (bit0 || bit1 || bit2 || bit3)
  {
    cpu.raise_interrupt(LR35902::Interrupt::JOYPAD);
  }

  // 0 means button is pressed, 1 means not pressed - invert all bits
  return ~JOYP;
}
