#pragma once

#include "types.h"

class LR35902;
class Memory;

class Joypad
{
  LR35902 &cpu;
  Memory &memory;

  bool buttons[8];

public:
  struct Button
  {
    enum Name
    {
      UP = 0,
      DOWN,
      LEFT,
      RIGHT,
      A,
      B,
      START,
      SELECT,
    };
  };

  Joypad() = delete;
  explicit Joypad(LR35902 &lr35902, Memory &mem);

  void button_pressed(Button::Name b);
  void button_released(Button::Name b);
  u8 get_button_state() const;
};
