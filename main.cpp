#include <stdio.h>
#include <unistd.h>
#include <fstream>

#include "gameboy.h"

static char *name;

void render_loop(Gameboy &gb);

void usage()
{
  printf("Usage: %s [options] rom\n", name);
  printf("Options:\n");
  printf("  -d  Run in debug mode\n");
}

int main(int argc, char *argv[])
{
  name = argv[0];
  if (argc < 2)
  {
    usage();
    return 1;
  }

  Gameboy gb;

  int c;
  while ((c = getopt(argc, argv, "d")) != -1)
  {
    switch (c)
    {
      case 'd':
        gb.set_debug(true);
        break;
      default:
        usage();
        return 1;
    }
  }
  if (optind != argc-1)
  {
    usage();
    return 1;
  }

  char *rom_file = argv[optind];

  std::ifstream rom(rom_file);
  if (!rom.is_open())
  {
    fprintf(stderr, "Couldn't load ROM from '%s'\n", rom_file);
    abort();
  }

  gb.load_rom(rom);

  render_loop(gb);

  return 0;
}
