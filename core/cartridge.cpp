#include "cartridge.h"
#include "gameboy.h"

void Cartridge::init_cartridge(std::istream& rom_stream, std::istream& ram_stream)
{
  // The cartridge header is located at 0x100 - 0x14f
  // Include the first 0x100 bytes here to make addressing easier
  const uint header_size = 0x150;
  char header[header_size];

  rom_stream.read(header, header_size);

  u8 cgb_flag       = header[0x143];
  u8 cartridge_type = header[0x147];
  u8 rom_size_code  = header[0x148];
  u8 ram_size_code  = header[0x149];

  if (cgb_flag & (1<<7))
  {
    if (cgb_flag & (1<<6))
    {
      // Game only works on CGB
      gb.gb_version = Gameboy::GB_VERSION_COLOUR;
      printf("Running in Gameboy Colour mode\n");
    }
    else
    {
      // Game supports colour and original - choose colour here
      // if user hasn't specified a preference
      if (gb.gb_version == Gameboy::GB_VERSION_UNDEFINED)
      {
        gb.gb_version = Gameboy::GB_VERSION_COLOUR;
        printf("Running in Gameboy Colour mode\n");
      }
    }
  }
  else
  {
    // Game only supports original
    gb.gb_version = Gameboy::GB_VERSION_ORIGINAL;
    printf("Running in original Gameboy mode\n");
  }

  init_mbc(cartridge_type);
  init_rom(rom_size_code);
  init_ram(ram_size_code);

  // Load the entire cartridge into our ROM buffer
  rom_stream.seekg(0);
  rom_stream.read(reinterpret_cast<char *>(rom.data()), rom.size());

  ram_stream.read(reinterpret_cast<char *>(ram.data()), ram.size());
}

void Cartridge::init_mbc(uint type)
{
  switch (type)
  {
    case 0x00:
      // ROM only
      mbc = std::make_unique<NoMBC>(rom, ram);
      break;
    case 0x01: case 0x02: case 0x03:
      // MBC1
      mbc = std::make_unique<MBC1>(rom, ram);
      break;
    case 0x0f: case 0x10: case 0x11: case 0x12: case 0x13:
      // MBC3
      mbc = std::make_unique<MBC3>(rom, ram);
      break;
    default:
      fprintf(stderr, "Unsupported cartridge type: %02X\n", type);
      abort();
  }
}

void Cartridge::init_rom(uint size_code)
{
  switch (size_code)
  {
    case 0x00:
      rom.resize(0x8000); // 32 KB
      break;
    case 0x01:
      rom.resize(0x10000); // 64 KB
      break;
    case 0x02:
      rom.resize(0x20000); // 128 KB
      break;
    case 0x03:
      rom.resize(0x40000); // 256 KB
      break;
    case 0x04:
      rom.resize(0x80000); // 512 KB
      break;
    case 0x05:
      rom.resize(0x100000); // 1 MB
      break;
    case 0x06:
      rom.resize(0x200000); // 2 MB
      break;
    case 0x07:
      rom.resize(0x400000); // 4 MB
      break;
    case 0x52:
      rom.resize(0x120000); // 1.125 MB
      break;
    case 0x53:
      rom.resize(0x140000); // 1.25 MB
      break;
    case 0x54:
      rom.resize(0x180000); // 1.5 MB
      break;
    default:
      fprintf(stderr, "Unsupported ROM size: %02X\n", size_code);
      abort();
  }
}

void Cartridge::init_ram(uint size_code)
{
  switch (size_code)
  {
    case 0x00:
      ram.resize(0); // None
      break;
    case 0x01:
      ram.resize(0x800); // 2 KB
      break;
    case 0x02:
      ram.resize(0x2000); // 8 KB
      break;
    case 0x03:
      ram.resize(0x8000); // 32 KB
      break;
    case 0x04:
      ram.resize(0x20000); // 128 KB
      break;
    case 0x05:
      ram.resize(0x10000); // 64 KB
      break;
    default:
      fprintf(stderr, "Unsupported RAM size: %02X\n", size_code);
      abort();
  }
}

void Cartridge::set_save_callback(MemoryBankController::SaveRAMCallback save_ram)
{
  mbc->save_ram_callback = save_ram;
}
