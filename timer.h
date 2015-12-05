#pragma once

#include "types.h"

class LR35902;
class Memory;

class Timer
{
  LR35902 &cpu;
  Memory &memory;

  const uint clock_speed = 0x400000; // 4 MHz
  const uint freq = 0x1000; // 4 kHz
  const uint cycles_per_tick = clock_speed / freq;

  int counter = cycles_per_tick;

public:
  Timer() = delete;
  explicit Timer(LR35902 &lr35902, Memory &mem) : cpu(lr35902), memory(mem) { }

  void update(uint cycles);
};
