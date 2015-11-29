#include "gameboy.h"

#include <fstream>

void Gameboy::load_rom(char *rom_file)
{
  std::ifstream rom(rom_file);
  if (!rom.is_open())
  {
    fprintf(stderr, "Couldn't load ROM from '%s'\n", rom_file);
    abort();
  }

  cart.load_rom(rom);
}

void Gameboy::run()
{
  cpu.run();
}

void Gameboy::set_debug(bool debug)
{
  cpu.debug = debug;
}
