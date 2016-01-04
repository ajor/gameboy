#pragma once

#include <vector>
#include "types.h"

class MemoryBankController
{
public:
  MemoryBankController() = delete;
  MemoryBankController(const std::vector<u8> &rom, std::vector<u8> &ram) : rom(rom), ram(ram) {}
  virtual ~MemoryBankController();

  virtual u8 get8(uint address) const = 0;
  virtual void set8(uint address, u8 value) = 0;

  using SaveRAMCallback = void(*)(void *ram, uint size);
  SaveRAMCallback save_ram_callback = nullptr;

protected:
  void save();

  const std::vector<u8> &rom;
  std::vector<u8> &ram;
  uint active_rom_bank = 1;
  uint active_ram_bank = 0;
  bool ram_enabled = false;
};

class NoMBC final : public MemoryBankController
{
public:
  using MemoryBankController::MemoryBankController;

  u8 get8(uint address) const override;
  void set8(uint address, u8 value) override;
};

class MBC1 final : public MemoryBankController
{
  enum class BankingMode
  {
    ROM,
    RAM,
  };

  BankingMode banking_mode = BankingMode::ROM;

public:
  using MemoryBankController::MemoryBankController;

  u8 get8(uint address) const override;
  void set8(uint address, u8 value) override;
};

class MBC3 final : public MemoryBankController
{
  /*
  struct RTCRegister
  {
    enum Reg
    {
      S  = 0,  // Seconds
      M  = 1,  // Minutes
      H  = 2,  // Hours
      DL = 3,  // Day (lower 8 bits)
      DH = 4,  // Day (upper 1 bit), Carry bit, Half flag
    };
  };
  */

  enum class BankingMode
  {
    RAM,
    RTC,
  };

  BankingMode banking_mode = BankingMode::RAM;

  uint active_rtc;
  u8 rtc[5];
  // TODO actually implement a clock

public:
  using MemoryBankController::MemoryBankController;

  u8 get8(uint address) const override;
  void set8(uint address, u8 value) override;
};
