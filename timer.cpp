#include "timer.h"
#include "lr35902.h"

void Timer::update(uint cycles)
{
  update_divider(cycles);

  // Should this be here, or updated when TAC is written to in Memory?
  update_clock_freq();

  if (interrupt_pending)
  {
    // An overflow occured on the previous cycle
    // Reset TIMA and raise an interrupt
    memory.set8(Memory::IO::TIMA, memory.get8(Memory::IO::TMA));
    cpu.raise_interrupt(LR35902::Interrupt::TIMER);
    interrupt_pending = false;
  }

  counter -= cycles;

  if (counter <= 0)
  {
    counter += cycles_per_tick;

    u8 TIMA = memory.get8(Memory::IO::TIMA);
    if (TIMA == 0xff)
    {
      // Overflow - raise an interrupt and reset TIMA on the next cycle
      interrupt_pending = true;
    }
    // 8-bit overflow expected:
    memory.set8(Memory::IO::TIMA, TIMA+1);
  }
}

void Timer::update_divider(uint cycles)
{
  divider_counter -= cycles;

  if (divider_counter <= 0)
  {
    divider_counter += divider_cycles_per_tick;

    // 8-bit overflow expected:
    memory.direct_io_write8(Memory::IO::DIV, memory.get8(Memory::IO::DIV)+1);
  }
}

bool Timer::timer_enabled()
{
  u8 TAC = memory.get8(Memory::IO::TAC);
  return TAC & 0x4;
}

void Timer::update_clock_freq()
{
  u8 TAC = memory.get8(Memory::IO::TAC);
  switch (TAC & 0x3)
  {
    case 0:
      cycles_per_tick = 1024; // 4 kHz
      break;
    case 1:
      cycles_per_tick = 16; // 256 kHz
      break;
    case 2:
      cycles_per_tick = 64; // 64 kHz
      break;
    case 3:
      cycles_per_tick = 256; // 16 kHz
      break;
    default:
      break;
  }
}
