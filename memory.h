#pragma once

#include <array>
#include <istream>
#include <string.h>

class Memory
{
  const static size_t size = 0x10000;
  uint8_t mem8[size];

public:
  void load(unsigned int address, std::size_t n, std::istream& src)
  {
    src.read(reinterpret_cast<char *>(&mem8[address]), n);
  }

  void load(unsigned int address, std::size_t n, uint8_t *src)
  {
    memcpy(&mem8[address], src, n);
  }

  void set8(unsigned int address, uint8_t value)
  {
    if (address >= size)
    {
      fprintf(stderr, "set8 address = %u\n", address);
      abort();
    }

    mem8[address] = value;
  }

  uint8_t get8(unsigned int address)
  {
    if (address >= size)
    {
      fprintf(stderr, "get8 address = %u\n", address);
      abort();
    }

    return mem8[address];
  }

  void set16(unsigned int address, uint16_t value)
  {
    if (address+1 >= size)
    {
      fprintf(stderr, "set16 address = %u\n", address);
      abort();
    }

    uint8_t upper = (value & 0xff00) >> 8;
    uint8_t lower = (value & 0x00ff);
    mem8[address] = lower;
    mem8[address+1] = upper;
  }

  uint16_t get16(unsigned int address)
  {
    if (address+1 >= size)
    {
      fprintf(stderr, "get16 address = %u\n", address);
      abort();
    }

    uint8_t lower = mem8[address];
    uint8_t upper = mem8[address+1];
    uint16_t value = (upper << 8) | lower;
    return value;
  }

  void print(unsigned int start, unsigned int range=10)
  {
    for (unsigned int i=start; i<start+range; i++)
    {
      printf("0x%04X: 0x%02X\n", i, mem8[i]);
    }
  }
};
