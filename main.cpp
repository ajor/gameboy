#include <stdio.h>
#include <unistd.h>

#include "gameboy.h"

static char *name;

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

  char *rom = argv[optind];
  gb.load_rom(rom);
  gb.run();
  return 0;
}
