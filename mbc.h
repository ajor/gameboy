#pragma once

#include <vector>
#include "types.h"

class MemoryBankController
{
public:
  MemoryBankController() = delete;
  MemoryBankController(const std::vector<u8> &rom, std::vector<u8> &ram) : rom(rom), ram(ram) {}
  virtual ~MemoryBankController() {}

  virtual u8 get8(uint address) const = 0;
  virtual void set8(uint address, u8 value) = 0;

protected:
  const std::vector<u8> &rom;
  std::vector<u8> &ram;
  uint active_rom_bank = 1;
  uint active_ram_bank = 0;
  bool ram_enabled = false;
};

class MBC1 final : public MemoryBankController
{
  enum class BankingMode
  {
    ROM,
    RAM
  };

  BankingMode banking_mode = BankingMode::ROM;

public:
  using MemoryBankController::MemoryBankController;

  u8 get8(uint address) const override;
  void set8(uint address, u8 value) override;
};
