#include "gameboy.h"

void Gameboy::load_rom(std::istream& rom)
{
  cart.init_cartridge(rom);
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
