#pragma once

#include "types.h"
#include "memory.h"

class LR35902
{
  struct
  {
    // TODO #ifdef to check endian-ness
    union {
      struct {
        u8 f;
        u8 a;
      };
      u16 af;
    };
    union {
      struct {
        u8 c;
        u8 b;
      };
      u16 bc;
    };
    union {
      struct {
        u8 e;
        u8 d;
      };
      u16 de;
    };
    union {
      struct {
        u8 l;
        u8 h;
      };
      u16 hl;
    };
    u16 sp;
    u16 pc;
  } reg;

  Memory<8192> &memory;

  typedef void (LR35902::*InstrFunc)();

  struct OpInfo
  {
    uint cycles;     // Number of cycles to execute
    uint length;     // Instruction length in bytes (including opcode)
    const char *str; // Disassembly of instruction
  };

  struct Instruction
  {
    u8 opcode;      // First byte of instruction
    InstrFunc func; // Function pointer to instruction implementation
    OpInfo opinfo;
  };

  void unknown_instruction();

  void LD_a_n();
  void LD_b_n();
  void LD_c_n();
  void LD_d_n();
  void LD_e_n();
  void LD_h_n();
  void LD_l_n();
  void LD_hl_n();

  void LD_a_a();
  void LD_a_b();
  void LD_a_c();
  void LD_a_d();
  void LD_a_e();
  void LD_a_h();
  void LD_a_l();
  void LD_a_hl();
  void LD_b_a();
  void LD_b_b();
  void LD_b_c();
  void LD_b_d();
  void LD_b_e();
  void LD_b_h();
  void LD_b_l();
  void LD_b_hl();
  void LD_c_a();
  void LD_c_b();
  void LD_c_c();
  void LD_c_d();
  void LD_c_e();
  void LD_c_h();
  void LD_c_l();
  void LD_c_hl();
  void LD_d_a();
  void LD_d_b();
  void LD_d_c();
  void LD_d_d();
  void LD_d_e();
  void LD_d_h();
  void LD_d_l();
  void LD_d_hl();
  void LD_e_a();
  void LD_e_b();
  void LD_e_c();
  void LD_e_d();
  void LD_e_e();
  void LD_e_h();
  void LD_e_l();
  void LD_e_hl();
  void LD_h_a();
  void LD_h_b();
  void LD_h_c();
  void LD_h_d();
  void LD_h_e();
  void LD_h_h();
  void LD_h_l();
  void LD_h_hl();
  void LD_l_a();
  void LD_l_b();
  void LD_l_c();
  void LD_l_d();
  void LD_l_e();
  void LD_l_h();
  void LD_l_l();
  void LD_l_hl();
  void LD_hl_a();
  void LD_hl_b();
  void LD_hl_c();
  void LD_hl_d();
  void LD_hl_e();
  void LD_hl_h();
  void LD_hl_l();

  void LD_bc_a();
  void LD_de_a();
  void LD_hl_inc_a();
  void LD_hl_dec_a();

  void LD_a_bc();
  void LD_a_de();
  void LD_a_hl_inc();
  void LD_a_hl_dec();

  void LDH_naddr_a();
  void LDH_a_naddr();
  void LD_caddr_a();
  void LD_a_caddr();
  void LD_naddr_a();
  void LD_a_naddr();

  void LD_bc_d16();
  void LD_de_d16();
  void LD_hl_d16();
  void LD_sp_d16();
  void LD_a16_sp();

  void POP_bc();
  void POP_de();
  void POP_hl();
  void POP_af();

  void PUSH_bc();
  void PUSH_de();
  void PUSH_hl();
  void PUSH_af();

  void LD_hl_sp_r8();
  void LD_sp_hl();

  static constexpr OpInfo unknown_info = {0, 0, "Unknown instruction"};
  static constexpr Instruction implemented_instruction_table[] = 
  {
    // 8-bit loads
    {0x06, &LR35902::LD_b_n, {8, 2, "LD B, n"}},
    {0x0e, &LR35902::LD_c_n, {8, 2, "LD C, n"}},
    {0x16, &LR35902::LD_d_n, {8, 2, "LD D, n"}},
    {0x1e, &LR35902::LD_e_n, {8, 2, "LD E, n"}},
    {0x26, &LR35902::LD_h_n, {8, 2, "LD H, n"}},
    {0x2e, &LR35902::LD_l_n, {8, 2, "LD L, n"}},
    {0x36, &LR35902::LD_hl_n,{12,2, "LD (HL), n"}},
    {0x3e, &LR35902::LD_a_n, {8, 2, "LD A, n"}},

    {0x02, &LR35902::LD_bc_a,     {8, 1, "LD (BC), A"}},
    {0x12, &LR35902::LD_de_a,     {8, 1, "LD (DE), A"}},
    {0x22, &LR35902::LD_hl_inc_a, {8, 1, "LD (HL+), A"}},
    {0x32, &LR35902::LD_hl_dec_a, {8, 1, "LD (HL-), A"}},

    {0x0a, &LR35902::LD_a_bc,     {8, 1, "LD A, (BC)"}},
    {0x1a, &LR35902::LD_a_de,     {8, 1, "LD A, (DE)"}},
    {0x2a, &LR35902::LD_a_hl_inc, {8, 1, "LD A, (HL+)"}},
    {0x3a, &LR35902::LD_a_hl_dec, {8, 1, "LD A, (HL-)"}},

    {0x78, &LR35902::LD_a_b,  {4, 1, "LD A, B"}},
    {0x79, &LR35902::LD_a_c,  {4, 1, "LD A, C"}},
    {0x7a, &LR35902::LD_a_d,  {4, 1, "LD A, D"}},
    {0x7b, &LR35902::LD_a_e,  {4, 1, "LD A, E"}},
    {0x7c, &LR35902::LD_a_h,  {4, 1, "LD A, H"}},
    {0x7d, &LR35902::LD_a_l,  {4, 1, "LD A, L"}},
    {0x7e, &LR35902::LD_a_hl, {8, 1, "LD A, (HL)"}},
    {0x7f, &LR35902::LD_a_a,  {4, 1, "LD A, A"}},
    {0x40, &LR35902::LD_b_b,  {4, 1, "LD B, B"}},
    {0x41, &LR35902::LD_b_c,  {4, 1, "LD B, C"}},
    {0x42, &LR35902::LD_b_d,  {4, 1, "LD B, D"}},
    {0x43, &LR35902::LD_b_e,  {4, 1, "LD B, E"}},
    {0x44, &LR35902::LD_b_h,  {4, 1, "LD B, H"}},
    {0x45, &LR35902::LD_b_l,  {4, 1, "LD B, L"}},
    {0x46, &LR35902::LD_b_hl, {8, 1, "LD B, (HL)"}},
    {0x47, &LR35902::LD_b_a,  {4, 1, "LD B, A"}},
    {0x48, &LR35902::LD_c_b,  {4, 1, "LD C, B"}},
    {0x49, &LR35902::LD_c_c,  {4, 1, "LD C, C"}},
    {0x4a, &LR35902::LD_c_d,  {4, 1, "LD C, D"}},
    {0x4b, &LR35902::LD_c_e,  {4, 1, "LD C, E"}},
    {0x4c, &LR35902::LD_c_h,  {4, 1, "LD C, H"}},
    {0x4d, &LR35902::LD_c_l,  {4, 1, "LD C, L"}},
    {0x4e, &LR35902::LD_c_hl, {8, 1, "LD C, (HL)"}},
    {0x4f, &LR35902::LD_c_a,  {4, 1, "LD C, A"}},
    {0x50, &LR35902::LD_d_b,  {4, 1, "LD D, B"}},
    {0x51, &LR35902::LD_d_c,  {4, 1, "LD D, C"}},
    {0x52, &LR35902::LD_d_d,  {4, 1, "LD D, D"}},
    {0x53, &LR35902::LD_d_e,  {4, 1, "LD D, E"}},
    {0x54, &LR35902::LD_d_h,  {4, 1, "LD D, H"}},
    {0x55, &LR35902::LD_d_l,  {4, 1, "LD D, L"}},
    {0x56, &LR35902::LD_d_hl, {8, 1, "LD D, (HL)"}},
    {0x57, &LR35902::LD_d_a,  {4, 1, "LD D, A"}},
    {0x58, &LR35902::LD_e_b,  {4, 1, "LD E, B"}},
    {0x59, &LR35902::LD_e_c,  {4, 1, "LD E, C"}},
    {0x5a, &LR35902::LD_e_d,  {4, 1, "LD E, D"}},
    {0x5b, &LR35902::LD_e_e,  {4, 1, "LD E, E"}},
    {0x5c, &LR35902::LD_e_h,  {4, 1, "LD E, H"}},
    {0x5d, &LR35902::LD_e_l,  {4, 1, "LD E, L"}},
    {0x5e, &LR35902::LD_e_hl, {8, 1, "LD E, (HL)"}},
    {0x5f, &LR35902::LD_e_a,  {4, 1, "LD E, A"}},
    {0x60, &LR35902::LD_h_b,  {4, 1, "LD H, B"}},
    {0x61, &LR35902::LD_h_c,  {4, 1, "LD H, C"}},
    {0x62, &LR35902::LD_h_d,  {4, 1, "LD H, D"}},
    {0x63, &LR35902::LD_h_e,  {4, 1, "LD H, E"}},
    {0x64, &LR35902::LD_h_h,  {4, 1, "LD H, H"}},
    {0x65, &LR35902::LD_h_l,  {4, 1, "LD H, L"}},
    {0x66, &LR35902::LD_h_hl, {8, 1, "LD H, (HL)"}},
    {0x67, &LR35902::LD_h_a,  {4, 1, "LD H, A"}},
    {0x68, &LR35902::LD_l_b,  {4, 1, "LD L, B"}},
    {0x69, &LR35902::LD_l_c,  {4, 1, "LD L, C"}},
    {0x6a, &LR35902::LD_l_d,  {4, 1, "LD L, D"}},
    {0x6b, &LR35902::LD_l_e,  {4, 1, "LD L, E"}},
    {0x6c, &LR35902::LD_l_h,  {4, 1, "LD L, H"}},
    {0x6d, &LR35902::LD_l_l,  {4, 1, "LD L, L"}},
    {0x6e, &LR35902::LD_l_hl, {8, 1, "LD L, (HL)"}},
    {0x6f, &LR35902::LD_l_a,  {4, 1, "LD L, A"}},
    {0x70, &LR35902::LD_hl_b, {8, 1, "LD (HL), B"}},
    {0x71, &LR35902::LD_hl_c, {8, 1, "LD (HL), C"}},
    {0x72, &LR35902::LD_hl_d, {8, 1, "LD (HL), D"}},
    {0x73, &LR35902::LD_hl_e, {8, 1, "LD (HL), E"}},
    {0x74, &LR35902::LD_hl_h, {8, 1, "LD (HL), H"}},
    {0x75, &LR35902::LD_hl_l, {8, 1, "LD (HL), L"}},
    {0x77, &LR35902::LD_hl_a, {8, 1, "LD (HL), A"}},

    {0xe0, &LR35902::LDH_naddr_a, {12, 2, "LDH (n), A"}},
    {0xf0, &LR35902::LDH_a_naddr, {12, 2, "LDH A, (n)"}},
    {0xe2, &LR35902::LD_caddr_a,  { 8, 2, "LD (C), A"}},
    {0xf2, &LR35902::LD_a_caddr,  { 8, 2, "LD A, (C)"}},
    {0xea, &LR35902::LD_naddr_a,  {16, 3, "LD (n), A"}},
    {0xfa, &LR35902::LD_a_naddr,  {16, 3, "LD A, (n)"}},

    // 16-bit loads
    {0x01, &LR35902::LD_bc_d16, {12, 3, "LD BD, n"}},
    {0x11, &LR35902::LD_de_d16, {12, 3, "LD DE, n"}},
    {0x21, &LR35902::LD_hl_d16, {12, 3, "LD HL, n"}},
    {0x31, &LR35902::LD_sp_d16, {12, 3, "LD SP, n"}},
    {0x08, &LR35902::LD_a16_sp, {20, 3, "LD (n), SP"}},

    {0xc1, &LR35902::POP_bc,    {12, 1, "POP BC"}},
    {0xd1, &LR35902::POP_de,    {12, 1, "POP DE"}},
    {0xe1, &LR35902::POP_hl,    {12, 1, "POP HL"}},
    {0xf1, &LR35902::POP_af,    {12, 1, "POP AF"}},

    {0xc5, &LR35902::PUSH_bc,   {16, 1, "PUSH BC"}},
    {0xd5, &LR35902::PUSH_de,   {16, 1, "PUSH DE"}},
    {0xe5, &LR35902::PUSH_hl,   {16, 1, "PUSH HL"}},
    {0xf5, &LR35902::PUSH_af,   {16, 1, "PUSH AF"}},

    {0xf8, &LR35902::LD_hl_sp_r8, {12, 2, "LD HL, SP+n"}},
    {0xf9, &LR35902::LD_sp_hl,    { 8, 1, "LD SP, HL"}},
  };

  static const int table_size = 0xff;
  static InstrFunc optable[table_size];
  static OpInfo infotable[table_size];

  void init_tables();

  void execute(u8 opcode);

public:
  LR35902() = delete;
  LR35902(Memory<8192> &mem) : memory(mem)
  {
    init_tables();
  }

  void run();
};
