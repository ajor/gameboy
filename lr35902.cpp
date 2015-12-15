#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "lr35902.h"
#include "memory.h"
#include "display.h"

LR35902::InstrFunc LR35902::optable[LR35902::table_size];
LR35902::OpInfo LR35902::infotable[LR35902::table_size];
LR35902::InstrFunc LR35902::optable_cb[LR35902::table_size];
LR35902::OpInfo LR35902::infotable_cb[LR35902::table_size];
constexpr LR35902::OpInfo LR35902::unknown_info;
constexpr LR35902::Instruction LR35902::implemented_instruction_table[];
constexpr LR35902::Instruction LR35902::implemented_instruction_table_cb[];

uint LR35902::step()
{
    if (!stopped && !halted)
    {
      execute();
      // curr_instr_cycles when halted?
      reg.f &= 0xf0;
      timer.update(curr_instr_cycles);

      return curr_instr_cycles;
    }
    return 4; // 4 cycles when halted? - just made this up
}

void LR35902::execute()
{
  u8 opcode = memory.get8(reg.pc);
  if (debug)
  {
    printf("%04X: %02X - %s\n", reg.pc, opcode, infotable[opcode].str);
  }

  OpInfo info = infotable[opcode];
  reg.pc += info.length;
  curr_instr_cycles = info.cycles;

  InstrFunc instr = optable[opcode];
  (this->*instr)();
}

void LR35902::execute_cb()
{
  u8 opcode = memory.get8(reg.pc + 1);
  if (debug)
  {
    printf("%04X: %02X - %s\n", reg.pc, opcode, infotable_cb[opcode].str);
  }

  OpInfo info = infotable_cb[opcode];
  reg.pc += info.length;
  curr_instr_cycles = info.cycles;

  InstrFunc instr = optable_cb[opcode];
  (this->*instr)();
}

void LR35902::handle_interrupts()
{
  if (interrupt_master_enable)
  {
    if (get_interrupt_enable(0) && get_interrupt_flag(0))
    {
      // V-Blank
      halted = false;
      clear_interrupt_flag(0);
      call_interrupt_handler(0x40);
    }
    else if (get_interrupt_enable(1) && get_interrupt_flag(1))
    {
      // LCD STAT
      halted = false;
      clear_interrupt_flag(1);
      call_interrupt_handler(0x48);
    }
    else if (get_interrupt_enable(2) && get_interrupt_flag(2))
    {
      // Timer
      halted = false;
      clear_interrupt_flag(2);
      call_interrupt_handler(0x50);
    }
    else if (get_interrupt_enable(3) && get_interrupt_flag(3))
    {
      // Serial
      halted = false;
      clear_interrupt_flag(3);
      call_interrupt_handler(0x58);
    }
    else if (get_interrupt_enable(4) && get_interrupt_flag(4))
    {
      // Joypad
      halted = false;
      stopped = false;
      clear_interrupt_flag(4);
      call_interrupt_handler(0x60);
    }
  }
}

void LR35902::call_interrupt_handler(uint address)
{
  interrupt_master_enable = false;
  reg.sp -= 2;
  memory.set16(reg.sp, reg.pc);
  reg.pc = address;
}

void LR35902::raise_interrupt(Interrupt::Type interrupt)
{
  u8 IF = memory.get8(Memory::IO::IF);
  IF |= 1 << interrupt;
  memory.set8(Memory::IO::IF, IF);
}

bool LR35902::get_interrupt_enable(uint bit) const
{
  return memory.get8(Memory::IO::IE) & (1 << bit);
}

bool LR35902::get_interrupt_flag(uint bit) const
{
  return memory.get8(Memory::IO::IF) & (1 << bit);
}

void LR35902::clear_interrupt_flag(uint bit)
{
  u8 IF = memory.get8(Memory::IO::IF);
  memory.set8(Memory::IO::IF, IF & ~(1<<bit));
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

    optable_cb[i] = &LR35902::unknown_instruction;
    infotable_cb[i] = unknown_info;
  }

  for (const Instruction &instr : implemented_instruction_table)
  {
    assert(optable[instr.opcode] == &LR35902::unknown_instruction);

    optable[instr.opcode] = instr.func;
    infotable[instr.opcode] = instr.opinfo;
  }

  for (const Instruction &instr : implemented_instruction_table_cb)
  {
    assert(optable_cb[instr.opcode] == &LR35902::unknown_instruction);

    optable_cb[instr.opcode] = instr.func;
    infotable_cb[instr.opcode] = instr.opinfo;
  }
}

void LR35902::unknown_instruction()
{
  fprintf(stderr, "Unknown instruction: %02X at %04X\n", memory.get8(reg.pc), reg.pc);
  abort();
}

void LR35902::NOP()
{
  // Do nothing
}

void LR35902::STOP()
{
  // Turn off screen and do nothing until button pressed
  if (interrupt_master_enable)
  {
    stopped = true;
  }
}

void LR35902::HALT()
{
  // Do nothing until interrupt occurs
  if (interrupt_master_enable)
  {
    halted = true;
  }
}

void LR35902::DI()
{
  // TODO don't disable interrupts immediately
  interrupt_master_enable = false;
}

void LR35902::EI()
{
  // TODO don't enable interrupts immediately
  interrupt_master_enable = true;
}

void LR35902::LD_a_n()
{
  reg.a = memory.get8(reg.pc - 1);
}

void LR35902::LD_b_n()
{
  reg.b = memory.get8(reg.pc - 1);
}

void LR35902::LD_c_n()
{
  reg.c = memory.get8(reg.pc - 1);
}

void LR35902::LD_d_n()
{
  reg.d = memory.get8(reg.pc - 1);
}

void LR35902::LD_e_n()
{
  reg.e = memory.get8(reg.pc - 1);
}

void LR35902::LD_h_n()
{
  reg.h = memory.get8(reg.pc - 1);
}

void LR35902::LD_l_n()
{
  reg.l = memory.get8(reg.pc - 1);
}

void LR35902::LD_hl_n()
{
  u8 n = memory.get8(reg.pc - 1);
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
  u8 n = memory.get8(reg.pc - 1);
  memory.set8(0xff00+n, reg.a);
}

void LR35902::LDH_a_naddr()
{
  u8 n = memory.get8(reg.pc - 1);
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

void LR35902::LD_naddr_a()
{
  u16 n = memory.get16(reg.pc - 2);
  memory.set8(n, reg.a);
}

void LR35902::LD_a_naddr()
{
  u16 n = memory.get16(reg.pc - 2);
  reg.a = memory.get8(n);
}

void LR35902::LD_bc_d16()
{
  reg.bc = memory.get16(reg.pc - 2);
}

void LR35902::LD_de_d16()
{
  reg.de = memory.get16(reg.pc - 2);
}

void LR35902::LD_hl_d16()
{
  reg.hl = memory.get16(reg.pc - 2);
}

void LR35902::LD_sp_d16()
{
  reg.sp = memory.get16(reg.pc - 2);
}

void LR35902::LD_a16_sp()
{
  u16 n = memory.get16(reg.pc - 2);
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
  s8 n = memory.get8(reg.pc - 1);

  set_flag_z(false);
  set_flag_n(false);
  set_flag_h((reg.sp&0xf) + (n&0xf) > 0xf);
  set_flag_c((reg.sp&0xff) + (n&0xff) > 0xff);

  reg.hl = reg.sp + n;
}

void LR35902::LD_sp_hl()
{
  reg.sp = reg.hl;
}

template <u8 LR35902::Reg::*R>
void LR35902::INC_R()
{
  set_flag_z(((reg.*R + 1)&0xff) == 0);
  set_flag_n(false);
  set_flag_h((reg.*R&0xf) + 1 > 0xf);

  (reg.*R)++;
}

void LR35902::INC_hladdr()
{
  memory.set8(reg.hl, memory.get8(reg.hl)+1);
}

template <u8 LR35902::Reg::*R>
void LR35902::DEC_R()
{
  set_flag_z(((reg.*R - 1)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.*R&0xf) == 0);

  (reg.*R)--;
}

void LR35902::DEC_hladdr()
{
  memory.set8(reg.hl, memory.get8(reg.hl)-1);
}

void LR35902::DAA()
{
  // Decimal Adjust register A

  uint a = reg.a;
  if (get_flag_n())
  {
    // Decimal adjust after subtraction
    if (get_flag_h())
    {
      a = (a - 0x6) & 0xff;
    }
    if (get_flag_c())
    {
      a -= 0x60;
    }
  }
  else
  {
    // Decimal adjust after addition
    if (get_flag_h() || (a&0xf) > 0x9)
    {
      a += 0x6;
    }
    if (get_flag_c() || a > 0x9f)
    {
      a += 0x60;
    }
  }

  reg.a = a & 0xff;

  set_flag_z(reg.a == 0);
  set_flag_h(false);
  if ((a&0x100) == 0x100)
    set_flag_c(true);
}

void LR35902::SCF()
{
  // Set Carry Flag

  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(true);
}

void LR35902::CPL()
{
  // Complement A Register
  //
  set_flag_n(true);
  set_flag_h(true);

  reg.a = ~reg.a;
}

void LR35902::CCF()
{
  // Complement Carry Flag

  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(!get_flag_c());
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::ADD_R_R()
{
  set_flag_z(((reg.*R1 + reg.*R2)&0xff) == 0);
  set_flag_n(false);
  set_flag_h((reg.*R1&0xf) + (reg.*R2&0xf) > 0xf);
  set_flag_c(reg.*R1 + reg.*R2 > 0xff);

  reg.*R1 += reg.*R2;
}

void LR35902::ADD_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);

  set_flag_z(((reg.a + hl)&0xff) == 0);
  set_flag_n(false);
  set_flag_h((reg.a&0xf) + (hl&0xf) > 0xf);
  set_flag_c(reg.a + hl > 0xff);

  reg.a += hl;
}

void LR35902::ADD_a_d8()
{
  u8 n = memory.get8(reg.pc - 1);

  set_flag_z(((reg.a + n)&0xff) == 0);
  set_flag_n(false);
  set_flag_h((reg.a&0xf) + (n&0xf) > 0xf);
  set_flag_c(reg.a + n > 0xff);

  reg.a += n;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::ADC_R_R()
{
  uint flag_c = get_flag_c();

  set_flag_z(((reg.*R1 + reg.*R2 + flag_c)&0xff) == 0);
  set_flag_n(false);
  set_flag_h((reg.*R1&0xf) + (reg.*R2&0xf) + flag_c > 0xf);
  set_flag_c(reg.*R1 + reg.*R2 + flag_c > 0xff);

  reg.*R1 += reg.*R2 + flag_c;
}

void LR35902::ADC_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  uint flag_c = get_flag_c();

  set_flag_z(((reg.a + hl + flag_c)&0xff) == 0);
  set_flag_n(false);
  set_flag_h((reg.a&0xf) + (hl&0xf) + flag_c > 0xf);
  set_flag_c(reg.a + hl + flag_c > 0xff);

  reg.a += hl + flag_c;
}

void LR35902::ADC_a_d8()
{
  u8 n = memory.get8(reg.pc - 1);
  uint flag_c = get_flag_c();

  set_flag_z(((reg.a + n + flag_c)&0xff) == 0);
  set_flag_n(false);
  set_flag_h((reg.a&0xf) + (n&0xf) + get_flag_c() > 0xf);
  set_flag_c(reg.a + n + flag_c > 0xff);

  reg.a += n + flag_c;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::SUB_R_R()
{
  set_flag_z(((reg.*R1 - reg.*R2)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.*R1&0xf) < (reg.*R2&0xf));
  set_flag_c(reg.*R1 < reg.*R2);

  reg.*R1 -= reg.*R2;
}

void LR35902::SUB_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);

  set_flag_z(((reg.a - hl)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.a&0xf) < (hl&0xf));
  set_flag_c(reg.a < hl);

  reg.a -= hl;
}

void LR35902::SUB_a_d8()
{
  u8 n = memory.get8(reg.pc - 1);

  set_flag_z(((reg.a - n)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.a&0xf) < (n&0xf));
  set_flag_c(reg.a < n);

  reg.a -= n;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::SBC_R_R()
{
  uint flag_c = get_flag_c();

  set_flag_z(((reg.*R1 - reg.*R2 - flag_c)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.*R1&0xf) < (reg.*R2&0xf) + flag_c);
  set_flag_c(reg.*R1 < reg.*R2 + flag_c);

  reg.*R1 -= reg.*R2 + flag_c;
}

void LR35902::SBC_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  uint flag_c = get_flag_c();

  set_flag_z(((reg.a - hl - flag_c)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.a&0xf) < (hl&0xf) + flag_c);
  set_flag_c(reg.a < hl + flag_c);

  reg.a -= hl + flag_c;
}

void LR35902::SBC_a_d8()
{
  u8 n = memory.get8(reg.pc - 1);
  uint flag_c = get_flag_c();

  set_flag_z(((reg.a - n - flag_c)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.a&0xf) < (n&0xf) + flag_c);
  set_flag_c(reg.a < n + flag_c);

  reg.a -= n + flag_c;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::AND_R_R()
{
  reg.*R1 &= reg.*R2;

  set_flag_z(reg.*R1 == 0);
  set_flag_n(false);
  set_flag_h(true);
  set_flag_c(false);
}

void LR35902::AND_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  reg.a &= hl;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(true);
  set_flag_c(false);
}

void LR35902::AND_a_d8()
{
  u8 n = memory.get8(reg.pc - 1);
  reg.a &= n;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(true);
  set_flag_c(false);
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::XOR_R_R()
{
  reg.*R1 ^= reg.*R2;

  set_flag_z(reg.*R1 == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

void LR35902::XOR_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  reg.a ^= hl;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

void LR35902::XOR_a_d8()
{
  u8 n = memory.get8(reg.pc - 1);
  reg.a ^= n;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::OR_R_R()
{
  reg.*R1 |= reg.*R2;

  set_flag_z(reg.*R1 == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

void LR35902::OR_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  reg.a |= hl;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

void LR35902::OR_a_d8()
{
  u8 n = memory.get8(reg.pc - 1);
  reg.a |= n;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::CP_R_R()
{
  set_flag_z(((reg.*R1 - reg.*R2)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.*R1&0xf) < (reg.*R2&0xf));
  set_flag_c(reg.*R1 < reg.*R2);
}

void LR35902::CP_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);

  set_flag_z(((reg.a - hl)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.a&0xf) < (hl&0xf));
  set_flag_c(reg.a < hl);
}

void LR35902::CP_a_d8()
{
  u8 n = memory.get8(reg.pc - 1);

  set_flag_z(((reg.a - n)&0xff) == 0);
  set_flag_n(true);
  set_flag_h((reg.a&0xf) < (n&0xf));
  set_flag_c(reg.a < n);
}

template <u16 LR35902::Reg::*R>
void LR35902::INC_RR()
{
  (reg.*R)++;
}

template <u16 LR35902::Reg::*R>
void LR35902::DEC_RR()
{
  (reg.*R)--;
}

template <u16 LR35902::Reg::*R1, u16 LR35902::Reg::*R2>
void LR35902::ADD_RR_RR()
{
  set_flag_n(false);
  set_flag_h((reg.*R1&0xfff) + (reg.*R2&0xfff) > 0xfff);
  set_flag_c(reg.*R1 + reg.*R2 > 0xffff);

  reg.*R1 += reg.*R2;
}

void LR35902::ADD_SP_r8()
{
  s8 n = memory.get8(reg.pc - 1);

  set_flag_z(false);
  set_flag_n(false);
  set_flag_h((reg.sp&0xf) + (n&0xf) > 0xf);
  set_flag_c((reg.sp&0xff) + (n&0xff) > 0xff);

  reg.sp += n;
}

void LR35902::JR_NZ_r8()
{
  if (get_flag_z() == false)
  {
    s8 n = memory.get8(reg.pc - 1);
    reg.pc += n;
    curr_instr_cycles += 4;
  }
}

void LR35902::JR_NC_r8()
{
  if (get_flag_c() == false)
  {
    s8 n = memory.get8(reg.pc - 1);
    reg.pc += n;
    curr_instr_cycles += 4;
  }
}

void LR35902::JR_r8()
{
  s8 n = memory.get8(reg.pc - 1);
  reg.pc += n;
}

void LR35902::JR_Z_r8()
{
  if (get_flag_z() == true)
  {
    s8 n = memory.get8(reg.pc - 1);
    reg.pc += n;
    curr_instr_cycles += 4;
  }
}

void LR35902::JR_C_r8()
{
  if (get_flag_c() == true)
  {
    s8 n = memory.get8(reg.pc - 1);
    reg.pc += n;
    curr_instr_cycles += 4;
  }
}

void LR35902::JP_NZ_a16()
{
  if (get_flag_z() == false)
  {
    u16 n = memory.get16(reg.pc - 2);
    reg.pc = n;
    curr_instr_cycles += 4;
  }
}

void LR35902::JP_NC_a16()
{
  if (get_flag_c() == false)
  {
    u16 n = memory.get16(reg.pc - 2);
    reg.pc = n;
    curr_instr_cycles += 4;
  }
}

void LR35902::JP_a16()
{
  u16 n = memory.get16(reg.pc - 2);
  reg.pc = n;
}

void LR35902::JP_Z_a16()
{
  if (get_flag_z() == true)
  {
    u16 n = memory.get16(reg.pc - 2);
    reg.pc = n;
    curr_instr_cycles += 4;
  }
}

void LR35902::JP_C_a16()
{
  if (get_flag_c() == true)
  {
    u16 n = memory.get16(reg.pc - 2);
    reg.pc = n;
    curr_instr_cycles += 4;
  }
}

void LR35902::JP_hl()
{
  reg.pc = reg.hl;
}

void LR35902::CALL_a16()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.pc);

  u16 n = memory.get16(reg.pc - 2);
  reg.pc = n;
}

void LR35902::CALL_NZ_a16()
{
  if (get_flag_z() == false)
  {
    CALL_a16();
    curr_instr_cycles += 12;
  }
}

void LR35902::CALL_NC_a16()
{
  if (get_flag_c() == false)
  {
    CALL_a16();
    curr_instr_cycles += 12;
  }
}

void LR35902::CALL_Z_a16()
{
  if (get_flag_z() == true)
  {
    CALL_a16();
    curr_instr_cycles += 12;
  }
}

void LR35902::CALL_C_a16()
{
  if (get_flag_c() == true)
  {
    CALL_a16();
    curr_instr_cycles += 12;
  }
}

void LR35902::RET()
{
  u16 n = memory.get16(reg.sp);
  reg.sp += 2;

  reg.pc = n;
}

void LR35902::RETI()
{
  RET();
  interrupt_master_enable = true;
}

void LR35902::RET_NZ()
{
  if (get_flag_z() == false)
  {
    RET();
    curr_instr_cycles += 12;
  }
}

void LR35902::RET_NC()
{
  if (get_flag_c() == false)
  {
    RET();
    curr_instr_cycles += 12;
  }
}

void LR35902::RET_Z()
{
  if (get_flag_z() == true)
  {
    RET();
    curr_instr_cycles += 12;
  }
}

void LR35902::RET_C()
{
  if (get_flag_c() == true)
  {
    RET();
    curr_instr_cycles += 12;
  }
}

template <u8 n> void LR35902::RST()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.pc);

  reg.pc = n;
}

template <u8 LR35902::Reg::*R>
void LR35902::RLC()
{
  uint c = reg.*R >> 7;
  reg.*R = (reg.*R << 1) | c;

  set_flag_z(reg.*R == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

template <u8 LR35902::Reg::*R>
void LR35902::RL()
{
  uint c = reg.*R >> 7;
  reg.*R = (reg.*R << 1) | get_flag_c();

  set_flag_z(reg.*R == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

template <u8 LR35902::Reg::*R>
void LR35902::RRC()
{
  uint c = reg.*R & 1;
  reg.*R = (reg.*R >> 1) | (c << 7);

  set_flag_z(reg.*R == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

template <u8 LR35902::Reg::*R>
void LR35902::RR()
{
  uint c = reg.*R & 1;
  reg.*R = (reg.*R >> 1) | (get_flag_c() << 7);

  set_flag_z(reg.*R == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

void LR35902::RLC_hladdr()
{
  u8 val = memory.get8(reg.hl);

  uint c = val >> 7;
  val = (val << 1) | c;

  memory.set8(reg.hl, val);

  set_flag_z(val == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

void LR35902::RL_hladdr()
{
  u8 val = memory.get8(reg.hl);

  uint c = val >> 7;
  val = (val << 1) | get_flag_c();

  memory.set8(reg.hl, val);

  set_flag_z(val == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

void LR35902::RRC_hladdr()
{
  u8 val = memory.get8(reg.hl);

  uint c = val & 1;
  val = (val >> 1) | (c << 7);

  memory.set8(reg.hl, val);

  set_flag_z(val == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

void LR35902::RR_hladdr()
{
  u8 val = memory.get8(reg.hl);

  uint c = val & 1;
  val = (val >> 1) | (get_flag_c() << 7);

  memory.set8(reg.hl, val);

  set_flag_z(val == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

template <u8 LR35902::Reg::*R>
void LR35902::SLA()
{
  uint c = reg.*R >> 7;
  reg.*R = reg.*R << 1;

  set_flag_z(reg.*R == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

template <u8 LR35902::Reg::*R>
void LR35902::SRA()
{
  uint c = reg.*R & 1;
  uint extend_bit = reg.*R & 0x80;
  reg.*R = (reg.*R >> 1) | extend_bit;

  set_flag_z(reg.*R == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

template <u8 LR35902::Reg::*R>
void LR35902::SRL()
{
  uint c = reg.*R & 1;
  reg.*R = reg.*R >> 1;

  set_flag_z(reg.*R == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

void LR35902::SLA_hladdr()
{
  u8 val = memory.get8(reg.hl);

  uint c = val >> 7;
  val = val << 1;

  memory.set8(reg.hl, val);

  set_flag_z(val == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

void LR35902::SRA_hladdr()
{
  u8 val = memory.get8(reg.hl);

  uint c = val & 1;
  uint extend_bit = val & 0x80;
  val = (val >> 1) | extend_bit;

  memory.set8(reg.hl, val);

  set_flag_z(val == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

void LR35902::SRL_hladdr()
{
  u8 val = memory.get8(reg.hl);

  uint c = val & 1;
  val = val >> 1;

  memory.set8(reg.hl, val);

  set_flag_z(val == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(c);
}

template <u8 LR35902::Reg::*R>
void LR35902::SWAP()
{
  u8 upper = (reg.*R & 0xf0) >> 4;
  u8 lower = reg.*R & 0xf;
  reg.*R = (lower << 4) | upper;

  set_flag_z(reg.*R == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

void LR35902::SWAP_hladdr()
{
  u8 val = memory.get8(reg.hl);

  u8 upper = (val & 0xf0) >> 4;
  u8 lower = val & 0xf;
  val = (lower << 4) | upper;

  memory.set8(reg.hl, val);

  set_flag_z(val == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

template <u8 bit, u8 LR35902::Reg::*R>
void LR35902::BIT()
{
  u8 bit_mask = 1 << bit;

  set_flag_z((reg.*R & bit_mask) == 0);
  set_flag_n(false);
  set_flag_h(true);
}

template <u8 bit, u8 LR35902::Reg::*R>
void LR35902::RES()
{
  u8 bit_mask = (u8) ~(1 << bit);
  reg.*R &= bit_mask;
}

template <u8 bit, u8 LR35902::Reg::*R>
void LR35902::SET()
{
  u8 bit_mask = 1 << bit;
  reg.*R |= bit_mask;
}

template <u8 bit>
void LR35902::BIT_hladdr()
{
  u8 val = memory.get8(reg.hl);

  u8 bit_mask = 1 << bit;

  set_flag_z((val & bit_mask) == 0);
  set_flag_n(false);
  set_flag_h(true);
}

template <u8 bit>
void LR35902::RES_hladdr()
{
  u8 val = memory.get8(reg.hl);

  u8 bit_mask = (u8) ~(1 << bit);
  val &= bit_mask;

  memory.set8(reg.hl, val);
}

template <u8 bit>
void LR35902::SET_hladdr()
{
  u8 val = memory.get8(reg.hl);

  u8 bit_mask = 1 << bit;
  val |= bit_mask;

  memory.set8(reg.hl, val);
}
