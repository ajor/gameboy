#pragma once

#include <array>
#include <istream>
#include <string.h>

template <std::size_t size>
class Memory
{
  static_assert((size % 2)==0, "size must be a multiple of 2");

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
    if (address >=0 && address < size)
    {
      mem8[address] = value;
    }
    else
    {
      fprintf(stderr, "set8 address = %u\n", address);
      abort();
    }
  }

  uint8_t get8(unsigned int address)
  {
    if (address >=0 && address < size)
      return mem8[address];
    fprintf(stderr, "get8 address = %u\n", address);
    abort();
  }

  void set16(unsigned int address, uint16_t value)
  {
    if (address >=0 && address+1 < size)
    {
      uint8_t upper = (value & 0xff00) >> 8;
      uint8_t lower = (value & 0x00ff);
      mem8[address] = upper;
      mem8[address+1] = lower;
    }
    else
    {
      fprintf(stderr, "set16 address = %u\n", address);
      abort();
    }
  }

  uint16_t get16(unsigned int address)
  {
    if (address >=0 && address+1 < size)
    {
      uint8_t upper = mem8[address];
      uint8_t lower = mem8[address+1];
      uint16_t value = (upper << 8) | lower;
      return value;
    }
    fprintf(stderr, "get16 address = %u\n", address);
    abort();
  }

  void print(unsigned int start, unsigned int range=10)
  {
    for (unsigned int i=start; i<start+range; i++)
    {
      printf("0x%04X: 0x%02X\n", i, mem8[i]);
    }
  }
};
