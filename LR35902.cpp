#include <stdio.h>

#include "LR35902.h"
#include "memory.h"

LR35902::InstrFunc LR35902::optable[LR35902::table_size];
LR35902::OpInfo LR35902::infotable[LR35902::table_size];
constexpr LR35902::OpInfo LR35902::unknown_info;
constexpr LR35902::Instruction LR35902::implemented_instruction_table[];

void LR35902::run()
{
  reg.pc = 0x100;
  reg.sp = 0xfffe;
  while (true)
  {
    uint8_t opcode = memory.get8(reg.pc);
    execute(opcode);

    // TODO deal with interrupts here
  }
}

void LR35902::execute(uint8_t opcode)
{
  InstrFunc instr = optable[opcode];
  (this->*instr)();
  OpInfo info = infotable[opcode];
  reg.pc += info.length;
  // TODO count cycles here too
}

void LR35902::init_tables()
{
  static bool initialised = false;
  if (initialised)
    return;

  for (int i=0; i<table_size; i++)
  {
    optable[i] = &LR35902::unknown_instruction;
    infotable[i] = unknown_info;
  }

  for (const Instruction &instr : implemented_instruction_table)
  {
    optable[instr.opcode] = instr.func;
    infotable[instr.opcode] = instr.opinfo;
  }
}

void LR35902::unknown_instruction()
{
  fprintf(stderr, "Unknown instruction: %02X at %04X\n", memory.get8(reg.pc), reg.pc);
  abort();
}

void LR35902::LD_a_n()
{
  reg.a = memory.get8(reg.pc + 1);
}

void LR35902::LD_b_n()
{
  reg.b = memory.get8(reg.pc + 1);
}

void LR35902::LD_c_n()
{
  reg.c = memory.get8(reg.pc + 1);
}

void LR35902::LD_d_n()
{
  reg.d = memory.get8(reg.pc + 1);
}

void LR35902::LD_e_n()
{
  reg.e = memory.get8(reg.pc + 1);
}

void LR35902::LD_h_n()
{
  reg.h = memory.get8(reg.pc + 1);
}

void LR35902::LD_l_n()
{
  reg.l = memory.get8(reg.pc + 1);
}

void LR35902::LD_hl_n()
{
  u8 n = memory.get8(reg.pc + 1);
  memory.set8(reg.hl, n);
}

void LR35902::LD_a_a()
{
  reg.a = reg.a;
}

void LR35902::LD_a_b()
{
  reg.a = reg.b;
}

void LR35902::LD_a_c()
{
  reg.a = reg.c;
}

void LR35902::LD_a_d()
{
  reg.a = reg.d;
}

void LR35902::LD_a_e()
{
  reg.a = reg.e;
}

void LR35902::LD_a_h()
{
  reg.a = reg.h;
}

void LR35902::LD_a_l()
{
  reg.a = reg.l;
}

void LR35902::LD_a_hl()
{
  reg.a = memory.get8(reg.hl);
}

void LR35902::LD_b_a()
{
  reg.b = reg.a;
}

void LR35902::LD_b_b()
{
  reg.b = reg.b;
}

void LR35902::LD_b_c()
{
  reg.b = reg.c;
}

void LR35902::LD_b_d()
{
  reg.b = reg.d;
}

void LR35902::LD_b_e()
{
  reg.b = reg.e;
}

void LR35902::LD_b_h()
{
  reg.b = reg.h;
}

void LR35902::LD_b_l()
{
  reg.b = reg.l;
}

void LR35902::LD_b_hl()
{
  reg.b = memory.get8(reg.hl);
}

void LR35902::LD_c_a()
{
  reg.c = reg.a;
}

void LR35902::LD_c_b()
{
  reg.c = reg.b;
}

void LR35902::LD_c_c()
{
  reg.c = reg.c;
}

void LR35902::LD_c_d()
{
  reg.c = reg.d;
}

void LR35902::LD_c_e()
{
  reg.c = reg.e;
}

void LR35902::LD_c_h()
{
  reg.c = reg.h;
}

void LR35902::LD_c_l()
{
  reg.c = reg.l;
}

void LR35902::LD_c_hl()
{
  reg.c = memory.get8(reg.hl);
}

void LR35902::LD_d_a()
{
  reg.d = reg.a;
}

void LR35902::LD_d_b()
{
  reg.d = reg.b;
}

void LR35902::LD_d_c()
{
  reg.d = reg.c;
}

void LR35902::LD_d_d()
{
  reg.d = reg.d;
}

void LR35902::LD_d_e()
{
  reg.d = reg.e;
}

void LR35902::LD_d_h()
{
  reg.d = reg.h;
}

void LR35902::LD_d_l()
{
  reg.d = reg.l;
}

void LR35902::LD_d_hl()
{
  reg.d = memory.get8(reg.hl);
}

void LR35902::LD_e_a()
{
  reg.e = reg.a;
}

void LR35902::LD_e_b()
{
  reg.e = reg.b;
}

void LR35902::LD_e_c()
{
  reg.e = reg.c;
}

void LR35902::LD_e_d()
{
  reg.e = reg.d;
}

void LR35902::LD_e_e()
{
  reg.e = reg.e;
}

void LR35902::LD_e_h()
{
  reg.e = reg.h;
}

void LR35902::LD_e_l()
{
  reg.e = reg.l;
}

void LR35902::LD_e_hl()
{
  reg.e = memory.get8(reg.hl);
}

void LR35902::LD_h_a()
{
  reg.h = reg.a;
}

void LR35902::LD_h_b()
{
  reg.h = reg.b;
}

void LR35902::LD_h_c()
{
  reg.h = reg.c;
}

void LR35902::LD_h_d()
{
  reg.h = reg.d;
}

void LR35902::LD_h_e()
{
  reg.h = reg.e;
}

void LR35902::LD_h_h()
{
  reg.h = reg.h;
}

void LR35902::LD_h_l()
{
  reg.h = reg.l;
}

void LR35902::LD_h_hl()
{
  reg.h = memory.get8(reg.hl);
}

void LR35902::LD_l_a()
{
  reg.l = reg.a;
}

void LR35902::LD_l_b()
{
  reg.l = reg.b;
}

void LR35902::LD_l_c()
{
  reg.l = reg.c;
}

void LR35902::LD_l_d()
{
  reg.l = reg.d;
}

void LR35902::LD_l_e()
{
  reg.l = reg.e;
}

void LR35902::LD_l_h()
{
  reg.l = reg.h;
}

void LR35902::LD_l_l()
{
  reg.l = reg.l;
}

void LR35902::LD_l_hl()
{
  reg.l = memory.get8(reg.hl);
}

void LR35902::LD_hl_a()
{
  memory.set8(reg.hl, reg.a);
}

void LR35902::LD_hl_b()
{
  memory.set8(reg.hl, reg.b);
}

void LR35902::LD_hl_c()
{
  memory.set8(reg.hl, reg.c);
}

void LR35902::LD_hl_d()
{
  memory.set8(reg.hl, reg.d);
}

void LR35902::LD_hl_e()
{
  memory.set8(reg.hl, reg.e);
}

void LR35902::LD_hl_h()
{
  memory.set8(reg.hl, reg.h);
}

void LR35902::LD_hl_l()
{
  memory.set8(reg.hl, reg.l);
}

void LR35902::LD_bc_a()
{
  memory.set8(reg.bc, reg.a);
}

void LR35902::LD_de_a()
{
  memory.set8(reg.de, reg.a);
}

void LR35902::LD_hl_inc_a()
{
  memory.set8(reg.hl++, reg.a);
}

void LR35902::LD_hl_dec_a()
{
  memory.set8(reg.hl--, reg.a);
}

void LR35902::LD_a_bc()
{
  reg.a = memory.get8(reg.bc);
}

void LR35902::LD_a_de()
{
  reg.a = memory.get8(reg.de);
}

void LR35902::LD_a_hl_inc()
{
  reg.a = memory.get8(reg.hl++);
}

void LR35902::LD_a_hl_dec()
{
  reg.a = memory.get8(reg.hl--);
}

void LR35902::LDH_naddr_a()
{
  u8 n = memory.get8(reg.pc + 1);
  memory.set8(0xff00+n, reg.a);
}

void LR35902::LDH_a_naddr()
{
  u8 n = memory.get8(reg.pc + 1);
  reg.a = memory.get8(0xff00+n);
}

void LR35902::LD_caddr_a()
{
  memory.set8(0xff00+reg.c, reg.a);
}

void LR35902::LD_a_caddr()
{
  reg.a = memory.get8(0xff00+reg.c);
}

// TODO check endianess for 16-bit loads
void LR35902::LD_naddr_a()
{
  u16 n = memory.get16(reg.pc + 1);
  memory.set8(n, reg.a);
}

void LR35902::LD_a_naddr()
{
  u16 n = memory.get16(reg.pc + 1);
  reg.a = memory.get8(n);
}

void LR35902::LD_bc_d16()
{
  reg.bc = memory.get16(reg.pc + 1);
}

void LR35902::LD_de_d16()
{
  reg.de = memory.get16(reg.pc + 1);
}

void LR35902::LD_hl_d16()
{
  reg.hl = memory.get16(reg.pc + 1);
}

void LR35902::LD_sp_d16()
{
  reg.sp = memory.get16(reg.pc + 1);
}

void LR35902::LD_a16_sp()
{
  u16 n = memory.get16(reg.pc + 1);
  memory.set16(n, reg.sp);
}

void LR35902::POP_bc()
{
  reg.bc = memory.get16(reg.sp);
  reg.sp += 2;
}

void LR35902::POP_de()
{
  reg.de = memory.get16(reg.sp);
  reg.sp += 2;
}

void LR35902::POP_hl()
{
  reg.hl = memory.get16(reg.sp);
  reg.sp += 2;
}

void LR35902::POP_af()
{
  reg.af = memory.get16(reg.sp);
  reg.sp += 2;
}

void LR35902::PUSH_bc()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.bc);
}

void LR35902::PUSH_de()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.de);
}

void LR35902::PUSH_hl()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.hl);
}

void LR35902::PUSH_af()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.af);
}

void LR35902::LD_hl_sp_r8()
{
  s8 n = memory.get8(reg.pc + 1);
  reg.hl = reg.sp + n;
}

void LR35902::LD_sp_hl()
{
  reg.sp = reg.hl;
}

void LR35902::INC_a()
{
  reg.a++;
}

void LR35902::INC_b()
{
  reg.b++;
}

void LR35902::INC_c()
{
  reg.c++;
}

void LR35902::INC_d()
{
  reg.d++;
}

void LR35902::INC_e()
{
  reg.e++;
}

void LR35902::INC_h()
{
  reg.h++;
}

void LR35902::INC_l()
{
  reg.l++;
}

void LR35902::INC_hladdr()
{
  memory.set8(reg.hl, memory.get8(reg.hl)+1);
}

void LR35902::DEC_a()
{
  reg.a--;
}

void LR35902::DEC_b()
{
  reg.b--;
}

void LR35902::DEC_c()
{
  reg.c--;
}

void LR35902::DEC_d()
{
  reg.d--;
}

void LR35902::DEC_e()
{
  reg.e--;
}

void LR35902::DEC_h()
{
  reg.h--;
}

void LR35902::DEC_l()
{
  reg.l--;
}

void LR35902::DEC_hladdr()
{
  memory.set8(reg.hl, memory.get8(reg.hl)-1);
}

void LR35902::DAA()
{
  unknown_instruction();
}

void LR35902::SCF()
{
  unknown_instruction();
}

void LR35902::CPL()
{
  unknown_instruction();
}

void LR35902::CCF()
{
  unknown_instruction();
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::ADD_R_R()
{
  clear_flag_n();
  if (reg.*R1 + reg.*R2 > 0xff)            set_flag_c();
  if ((reg.*R1&0xf) + (reg.*R2&0xf) > 0xf) set_flag_h();
  if (reg.*R1 + reg.*R2 == 0)              set_flag_z();

  reg.*R1 += reg.*R2;
}

void LR35902::ADD_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);

  clear_flag_n();
  if (reg.a + hl > 0xff)            set_flag_c();
  if ((reg.a&0xf) + (hl&0xf) > 0xf) set_flag_h();
  if (reg.a + hl == 0)              set_flag_z();

  reg.a += hl;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::ADC_R_R()
{
  uint add = reg.*R2 + get_flag_c();

  clear_flag_n();
  if (reg.*R1 + add > 0xff)            set_flag_c();
  if ((reg.*R1&0xf) + (add&0xf) > 0xf) set_flag_h();
  if (reg.*R1 + add == 0)              set_flag_z();

  reg.*R1 += add;
}

void LR35902::ADC_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  uint add = hl + get_flag_c();

  clear_flag_n();
  if (reg.a + add > 0xff)            set_flag_c();
  if ((reg.a&0xf) + (add&0xf) > 0xf) set_flag_h();
  if (reg.a + add == 0)              set_flag_z();

  reg.a += add;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::SUB_R_R()
{
  set_flag_n();
  if (reg.*R1 >= reg.*R2)             set_flag_c();
  if ((reg.*R1&0xf) >= (reg.*R2&0xf)) set_flag_h();
  if (reg.*R1 - reg.*R2 == 0)         set_flag_z();

  reg.*R1 -= reg.*R2;
}

void LR35902::SUB_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);

  set_flag_n();
  if (reg.a >= hl)             set_flag_c();
  if ((reg.a&0xf) >= (hl&0xf)) set_flag_h();
  if (reg.a - hl == 0)         set_flag_z();

  reg.a -= hl;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::SBC_R_R()
{
  uint sub = reg.*R2 + get_flag_c();

  set_flag_n();
  if (reg.*R1 >= sub)             set_flag_c();
  if ((reg.*R1&0xf) >= (sub&0xf)) set_flag_h();
  if (reg.*R1 - sub == 0)         set_flag_z();

  reg.*R1 -= sub;
}

void LR35902::SBC_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  uint sub = hl + get_flag_c();

  set_flag_n();
  if (reg.a >= sub)             set_flag_c();
  if ((reg.a&0xf) >= (sub&0xf)) set_flag_h();
  if (reg.a - sub == 0)         set_flag_z();

  reg.a -= sub;
}
