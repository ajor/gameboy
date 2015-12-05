#pragma once

#include "types.h"

class LR35902;
class Memory;

class Timer
{
  LR35902 &cpu;
  Memory &memory;

  const uint clock_speed = 0x400000; // 4 MHz
  const uint divider_cycles_per_tick = clock_speed / 0x4000; // 16 kHz
  uint cycles_per_tick = 1024;

  // Number of cycles remaining until we increment the timer
  int counter = cycles_per_tick;
  int divider_counter = divider_cycles_per_tick;

  // Timer interrupt is delayed for 1 cycle after TIMA overflows
  bool interrupt_pending = false;

  void update_divider(uint cycles);
  bool timer_enabled();
  void update_clock_freq();

public:
  Timer() = delete;
  explicit Timer(LR35902 &lr35902, Memory &mem) : cpu(lr35902), memory(mem) { }

  void update(uint cycles);
};
