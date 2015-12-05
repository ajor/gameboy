#include "timer.h"
#include "lr35902.h"

void Timer::update(uint cycles)
{
  counter -= cycles;

  if (counter <= 0)
  {
    counter += cycles_per_tick;
    cpu.raise_interrupt(LR35902::Interrupt::TIMER);
  }
}
