#include "gameboy.h"

void Gameboy::load_rom(std::istream& rom, std::istream& ram)
{
  cart.init_cartridge(rom, ram);
}

void Gameboy::set_save_callback(MemoryBankController::SaveRAMCallback save_ram)
{
  cart.set_save_callback(save_ram);
}

void Gameboy::run_to_vblank()
{
  while (!display.in_vblank())
  {
    step();
  }
  while (display.in_vblank())
  {
    step();
  }
}

void Gameboy::step()
{
  uint cycles = cpu.step();
  display.update(cycles);
  cpu.handle_interrupts();
}

void Gameboy::set_debug(bool debug)
{
  cpu.debug = debug;
}
