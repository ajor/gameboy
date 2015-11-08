#pragma once

#include "types.h"
#include "memory.h"

class LR35902
{
  struct Reg
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

  void set_flag_z(bool set) { if (set) reg.f |=  (1<<7); else reg.f &= ~(1<<7); }
  void set_flag_n(bool set) { if (set) reg.f |=  (1<<6); else reg.f &= ~(1<<6); }
  void set_flag_h(bool set) { if (set) reg.f |=  (1<<5); else reg.f &= ~(1<<5); }
  void set_flag_c(bool set) { if (set) reg.f |=  (1<<4); else reg.f &= ~(1<<4); }
  uint get_flag_z() const { return (reg.f >> 7)&1; }
  uint get_flag_n() const { return (reg.f >> 6)&1; }
  uint get_flag_h() const { return (reg.f >> 5)&1; }
  uint get_flag_c() const { return (reg.f >> 4)&1; }

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

  template <u8 LR35902::Reg::*R> void INC_R();
  void INC_hladdr();

  template <u8 LR35902::Reg::*R> void DEC_R();
  void DEC_hladdr();

  void DAA();
  void SCF();
  void CPL();
  void CCF();

  template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2> void ADD_R_R();
  template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2> void ADC_R_R();
  template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2> void SUB_R_R();
  template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2> void SBC_R_R();
  void ADD_a_hladdr();
  void ADC_a_hladdr();
  void SUB_a_hladdr();
  void SBC_a_hladdr();

  template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2> void AND_R_R();
  template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2> void XOR_R_R();
  template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2> void OR_R_R();
  template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2> void CP_R_R();
  void AND_a_hladdr();
  void XOR_a_hladdr();
  void OR_a_hladdr();
  void CP_a_hladdr();

  void ADD_a_d8();
  void SUB_a_d8();
  void AND_a_d8();
  void OR_a_d8();

  void ADC_a_d8();
  void SBC_a_d8();
  void XOR_a_d8();
  void CP_a_d8();

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

    // 8-bit ALU
    {0x04, &LR35902::INC_R<&LR35902::Reg::b>, { 4, 1, "INC B"}},
    {0x14, &LR35902::INC_R<&LR35902::Reg::d>, { 4, 1, "INC D"}},
    {0x24, &LR35902::INC_R<&LR35902::Reg::h>, { 4, 1, "INC H"}},
    {0x34, &LR35902::INC_hladdr,              {12, 1, "INC (HL)"}},

    {0x05, &LR35902::DEC_R<&LR35902::Reg::b>, { 4, 1, "DEC B"}},
    {0x15, &LR35902::DEC_R<&LR35902::Reg::d>, { 4, 1, "DEC D"}},
    {0x25, &LR35902::DEC_R<&LR35902::Reg::h>, { 4, 1, "DEC H"}},
    {0x35, &LR35902::DEC_hladdr,              {12, 1, "DEC (HL)"}},

    {0x27, &LR35902::DAA, {4, 1, "DAA"}},
    {0x37, &LR35902::SCF, {4, 1, "SCF"}},

    {0x0c, &LR35902::INC_R<&LR35902::Reg::c>, {4, 1, "INC C"}},
    {0x1c, &LR35902::INC_R<&LR35902::Reg::e>, {4, 1, "INC E"}},
    {0x2c, &LR35902::INC_R<&LR35902::Reg::l>, {4, 1, "INC L"}},
    {0x3c, &LR35902::INC_R<&LR35902::Reg::a>, {4, 1, "INC A"}},

    {0x0d, &LR35902::DEC_R<&LR35902::Reg::c>, {4, 1, "DEC C"}},
    {0x1d, &LR35902::DEC_R<&LR35902::Reg::e>, {4, 1, "DEC E"}},
    {0x2d, &LR35902::DEC_R<&LR35902::Reg::l>, {4, 1, "DEC L"}},
    {0x3d, &LR35902::DEC_R<&LR35902::Reg::a>, {4, 1, "DEC A"}},

    {0x2f, &LR35902::CPL, {4, 1, "CPL"}},
    {0x3f, &LR35902::CCF, {4, 1, "CCF"}},

    {0x80, &LR35902::ADD_R_R<&LR35902::Reg::a, &LR35902::Reg::b>, {4, 1, "ADD A, B"}},
    {0x81, &LR35902::ADD_R_R<&LR35902::Reg::a, &LR35902::Reg::c>, {4, 1, "ADD A, C"}},
    {0x82, &LR35902::ADD_R_R<&LR35902::Reg::a, &LR35902::Reg::d>, {4, 1, "ADD A, D"}},
    {0x83, &LR35902::ADD_R_R<&LR35902::Reg::a, &LR35902::Reg::e>, {4, 1, "ADD A, E"}},
    {0x84, &LR35902::ADD_R_R<&LR35902::Reg::a, &LR35902::Reg::h>, {4, 1, "ADD A, H"}},
    {0x85, &LR35902::ADD_R_R<&LR35902::Reg::a, &LR35902::Reg::l>, {4, 1, "ADD A, L"}},
    {0x86, &LR35902::ADD_a_hladdr,                                {8, 1, "ADD A, (HL)"}},
    {0x87, &LR35902::ADD_R_R<&LR35902::Reg::a, &LR35902::Reg::a>, {4, 1, "ADD A, A"}},

    {0x88, &LR35902::ADC_R_R<&LR35902::Reg::a, &LR35902::Reg::b>, {4, 1, "ADC A, B"}},
    {0x89, &LR35902::ADC_R_R<&LR35902::Reg::a, &LR35902::Reg::c>, {4, 1, "ADC A, C"}},
    {0x8a, &LR35902::ADC_R_R<&LR35902::Reg::a, &LR35902::Reg::d>, {4, 1, "ADC A, D"}},
    {0x8b, &LR35902::ADC_R_R<&LR35902::Reg::a, &LR35902::Reg::e>, {4, 1, "ADC A, E"}},
    {0x8c, &LR35902::ADC_R_R<&LR35902::Reg::a, &LR35902::Reg::h>, {4, 1, "ADC A, H"}},
    {0x8d, &LR35902::ADC_R_R<&LR35902::Reg::a, &LR35902::Reg::l>, {4, 1, "ADC A, L"}},
    {0x8e, &LR35902::ADC_a_hladdr,                                {8, 1, "ADC A, (HL)"}},
    {0x8f, &LR35902::ADC_R_R<&LR35902::Reg::a, &LR35902::Reg::a>, {4, 1, "ADC A, A"}},

    {0x90, &LR35902::SUB_R_R<&LR35902::Reg::a, &LR35902::Reg::b>, {4, 1, "SUB A, B"}},
    {0x91, &LR35902::SUB_R_R<&LR35902::Reg::a, &LR35902::Reg::c>, {4, 1, "SUB A, C"}},
    {0x92, &LR35902::SUB_R_R<&LR35902::Reg::a, &LR35902::Reg::d>, {4, 1, "SUB A, D"}},
    {0x93, &LR35902::SUB_R_R<&LR35902::Reg::a, &LR35902::Reg::e>, {4, 1, "SUB A, E"}},
    {0x94, &LR35902::SUB_R_R<&LR35902::Reg::a, &LR35902::Reg::h>, {4, 1, "SUB A, H"}},
    {0x95, &LR35902::SUB_R_R<&LR35902::Reg::a, &LR35902::Reg::l>, {4, 1, "SUB A, L"}},
    {0x96, &LR35902::SUB_a_hladdr,                                {8, 1, "SUB A, (HL)"}},
    {0x97, &LR35902::SUB_R_R<&LR35902::Reg::a, &LR35902::Reg::a>, {4, 1, "SUB A, A"}},

    {0x98, &LR35902::SBC_R_R<&LR35902::Reg::a, &LR35902::Reg::b>, {4, 1, "SBC A, B"}},
    {0x99, &LR35902::SBC_R_R<&LR35902::Reg::a, &LR35902::Reg::c>, {4, 1, "SBC A, C"}},
    {0x9a, &LR35902::SBC_R_R<&LR35902::Reg::a, &LR35902::Reg::d>, {4, 1, "SBC A, D"}},
    {0x9b, &LR35902::SBC_R_R<&LR35902::Reg::a, &LR35902::Reg::e>, {4, 1, "SBC A, E"}},
    {0x9c, &LR35902::SBC_R_R<&LR35902::Reg::a, &LR35902::Reg::h>, {4, 1, "SBC A, H"}},
    {0x9d, &LR35902::SBC_R_R<&LR35902::Reg::a, &LR35902::Reg::l>, {4, 1, "SBC A, L"}},
    {0x9e, &LR35902::SBC_a_hladdr,                                {8, 1, "SBC A, (HL)"}},
    {0x9f, &LR35902::SBC_R_R<&LR35902::Reg::a, &LR35902::Reg::a>, {4, 1, "SBC A, A"}},

    {0xa0, &LR35902::AND_R_R<&LR35902::Reg::a, &LR35902::Reg::b>, {4, 1, "AND A, B"}},
    {0xa1, &LR35902::AND_R_R<&LR35902::Reg::a, &LR35902::Reg::c>, {4, 1, "AND A, C"}},
    {0xa2, &LR35902::AND_R_R<&LR35902::Reg::a, &LR35902::Reg::d>, {4, 1, "AND A, D"}},
    {0xa3, &LR35902::AND_R_R<&LR35902::Reg::a, &LR35902::Reg::e>, {4, 1, "AND A, E"}},
    {0xa4, &LR35902::AND_R_R<&LR35902::Reg::a, &LR35902::Reg::h>, {4, 1, "AND A, H"}},
    {0xa5, &LR35902::AND_R_R<&LR35902::Reg::a, &LR35902::Reg::l>, {4, 1, "AND A, L"}},
    {0xa6, &LR35902::AND_a_hladdr,                                {8, 1, "AND A, (HL)"}},
    {0xa7, &LR35902::AND_R_R<&LR35902::Reg::a, &LR35902::Reg::a>, {4, 1, "AND A, A"}},

    {0xa8, &LR35902::XOR_R_R<&LR35902::Reg::a, &LR35902::Reg::b>, {4, 1, "XOR A, B"}},
    {0xa9, &LR35902::XOR_R_R<&LR35902::Reg::a, &LR35902::Reg::c>, {4, 1, "XOR A, C"}},
    {0xaa, &LR35902::XOR_R_R<&LR35902::Reg::a, &LR35902::Reg::d>, {4, 1, "XOR A, D"}},
    {0xab, &LR35902::XOR_R_R<&LR35902::Reg::a, &LR35902::Reg::e>, {4, 1, "XOR A, E"}},
    {0xac, &LR35902::XOR_R_R<&LR35902::Reg::a, &LR35902::Reg::h>, {4, 1, "XOR A, H"}},
    {0xad, &LR35902::XOR_R_R<&LR35902::Reg::a, &LR35902::Reg::l>, {4, 1, "XOR A, L"}},
    {0xae, &LR35902::XOR_a_hladdr,                                {8, 1, "XOR A, (HL)"}},
    {0xaf, &LR35902::XOR_R_R<&LR35902::Reg::a, &LR35902::Reg::a>, {4, 1, "XOR A, A"}},

    {0xb0, &LR35902::OR_R_R<&LR35902::Reg::a, &LR35902::Reg::b>, {4, 1, "OR A, B"}},
    {0xb1, &LR35902::OR_R_R<&LR35902::Reg::a, &LR35902::Reg::c>, {4, 1, "OR A, C"}},
    {0xb2, &LR35902::OR_R_R<&LR35902::Reg::a, &LR35902::Reg::d>, {4, 1, "OR A, D"}},
    {0xb3, &LR35902::OR_R_R<&LR35902::Reg::a, &LR35902::Reg::e>, {4, 1, "OR A, E"}},
    {0xb4, &LR35902::OR_R_R<&LR35902::Reg::a, &LR35902::Reg::h>, {4, 1, "OR A, H"}},
    {0xb5, &LR35902::OR_R_R<&LR35902::Reg::a, &LR35902::Reg::l>, {4, 1, "OR A, L"}},
    {0xb6, &LR35902::OR_a_hladdr,                                {8, 1, "OR A, (HL)"}},
    {0xb7, &LR35902::OR_R_R<&LR35902::Reg::a, &LR35902::Reg::a>, {4, 1, "OR A, A"}},

    {0xb8, &LR35902::CP_R_R<&LR35902::Reg::a, &LR35902::Reg::b>, {4, 1, "CP A, B"}},
    {0xb9, &LR35902::CP_R_R<&LR35902::Reg::a, &LR35902::Reg::c>, {4, 1, "CP A, C"}},
    {0xba, &LR35902::CP_R_R<&LR35902::Reg::a, &LR35902::Reg::d>, {4, 1, "CP A, D"}},
    {0xbb, &LR35902::CP_R_R<&LR35902::Reg::a, &LR35902::Reg::e>, {4, 1, "CP A, E"}},
    {0xbc, &LR35902::CP_R_R<&LR35902::Reg::a, &LR35902::Reg::h>, {4, 1, "CP A, H"}},
    {0xbd, &LR35902::CP_R_R<&LR35902::Reg::a, &LR35902::Reg::l>, {4, 1, "CP A, L"}},
    {0xbe, &LR35902::CP_a_hladdr,                                {8, 1, "CP A, (HL)"}},
    {0xbf, &LR35902::CP_R_R<&LR35902::Reg::a, &LR35902::Reg::a>, {4, 1, "CP A, A"}},

    {0xc6, &LR35902::ADD_a_d8, {8, 2, "ADD A, d8"}},
    {0xd6, &LR35902::SUB_a_d8, {8, 2, "SUB A, d8"}},
    {0xe6, &LR35902::AND_a_d8, {8, 2, "AND A, d8"}},
    {0xf6, &LR35902::OR_a_d8,  {8, 2,  "OR A, d8"}},
    {0xce, &LR35902::ADC_a_d8, {8, 2, "ADC A, d8"}},
    {0xde, &LR35902::SBC_a_d8, {8, 2, "SBC A, d8"}},
    {0xee, &LR35902::XOR_a_d8, {8, 2, "XOR A, d8"}},
    {0xfe, &LR35902::CP_a_d8,  {8, 2,  "CP A, d8"}},
  };

  static const int table_size = 0xff;
  static InstrFunc optable[table_size];
  static OpInfo infotable[table_size];

  void init_tables();

  void execute(u8 opcode);

public:
  LR35902() = delete;
  explicit LR35902(Memory<8192> &mem) : memory(mem)
  {
    init_tables();
  }

  void run();
};
