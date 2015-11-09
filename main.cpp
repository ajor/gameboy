#include <stdio.h>

#include "gameboy.h"

static char *name;

void usage()
{
  printf("Usage: %s [options] rom\n", name);
}

int main(int argc, char *argv[])
{
  name = argv[0];
  if (argc < 2)
  {
    usage();
    abort();
  }

  Gameboy gb;
  gb.load_rom(argv[1]);
  gb.run();
  return 0;
}
