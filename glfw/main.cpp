#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>

#include "core/gameboy.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static char *name;
static std::string ram_file;
Gameboy gb;

void render_loop(Gameboy &gb);

void save_ram(void *ram, unsigned int size)
{
  std::ofstream out(ram_file);
  out.write(reinterpret_cast<char *>(ram), size);

#ifdef __EMSCRIPTEN__
  // Sync the browser's persistent data storage with Emscripten's filesystem
  EM_ASM(
    FS.syncfs(false, function(err) {
      if (err) {
        console.error("Error saving game: " + err);
      }
    });
  );
#endif
}

void usage()
{
  printf("Usage: %s [options] rom\n", name);
  printf("Options:\n");
  printf("  -o file               Save game output file\n");
  printf("  -d [all|cpu|audio]    Run in debug mode\n");
  printf("  -v [original|colour]  Select version of Gameboy to emulate\n");
  printf("  -m                    Mute audio\n");
}

int main(int argc, char *argv[])
{
  name = argv[0];
  if (argc < 2)
  {
    usage();
    return 1;
  }

  bool ram_file_set = false;
  int c;
  while ((c = getopt(argc, argv, "d:v:o:m")) != -1)
  {
    switch (c)
    {
      case 'd':
      {
        std::string arg = optarg;
        if (arg == "all")
        {
          gb.set_debug(Gameboy::DEBUG_MODE::ALL, true);
        }
        else if (arg == "cpu")
        {
          gb.set_debug(Gameboy::DEBUG_MODE::CPU, true);
        }
        else if (arg == "audio")
        {
          gb.set_debug(Gameboy::DEBUG_MODE::AUDIO, true);
        }
        else
        {
          fprintf(stderr, "Invalid debug mode: '%s'\n", optarg);
          return 1;
        }
        break;
      }
      case 'v':
      {
        std::string arg = optarg;
        if (arg == "original")
        {
          gb.set_version(Gameboy::GB_VERSION::ORIGINAL);
        }
        else if (arg == "colour")
        {
          gb.set_version(Gameboy::GB_VERSION::COLOUR);
        }
        else
        {
          fprintf(stderr, "Invalid Gameboy version: '%s'\n", optarg);
          return 1;
        }
        break;
      }
      case 'o':
        ram_file = optarg;
        ram_file_set = true;
        break;
      case 'm':
        gb.set_muted(true);
        break;
      default:
        usage();
        return 1;
    }
  }

  // There should only be 1 non-option argument (the rom file)
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

  if (!ram_file_set)
  {
    // Save the RAM with the ROM if no other file has been specified
    ram_file = std::string(rom_file) + ".sav";
  }
  std::ifstream ram(ram_file);

  gb.load_rom(rom, ram);
  rom.close();
  ram.close();

  gb.set_save_callback(&save_ram);

  render_loop(gb);

  return 0;
}

#ifdef __EMSCRIPTEN__
extern "C"
{
  void save_game()
  {
    gb.save();
  }
}
#endif
