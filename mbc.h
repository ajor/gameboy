#pragma once

#include "types.h"

class MemoryBankController
{
public:
  MemoryBankController() = delete;
  MemoryBankController(u8 *rom, u8 *ram) : rom(rom), ram(ram) {}
  virtual ~MemoryBankController() {}

  virtual u8 get8(uint address) const = 0;
  virtual void set8(uint address, u8 value) = 0;

  enum class BankingMode
  {
    ROM,
    RAM
  };

protected:
  u8 *rom;
  u8 *ram;
  uint active_rom_bank = 1;
  uint active_ram_bank = 0;
  bool ram_enabled = false;
  BankingMode banking_mode = BankingMode::ROM;
};

class MBC1 final : public MemoryBankController
{
public:
  //using MemoryBankController::MemoryBankController;
  MBC1(u8 *rom, u8 *ram) : MemoryBankController(rom, ram) {}

  u8 get8(uint address) const override;
  void set8(uint address, u8 value) override;
};
