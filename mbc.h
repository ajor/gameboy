#pragma once

#include "types.h"

class MemoryBankController
{
protected:
  u8 *rom;
  u8 *ram;
  uint active_rom_bank_addr = 0x4000; // default to bank 1

public:
  MemoryBankController() = delete;
  explicit MemoryBankController(u8 *rom, u8 *ram) : rom(rom), ram(ram) {}
  virtual ~MemoryBankController() {}

  virtual u8 get8(uint address) const = 0;
  virtual void set8(uint address, u8 value) = 0;
};

class MBC1 final : public MemoryBankController
{
public:
  //using MemoryBankController::MemoryBankController;
  explicit MBC1(u8 *rom, u8 *ram) : MemoryBankController(rom, ram) {}

  u8 get8(uint address) const override;
  void set8(uint address, u8 value) override;
};
