#include <stdio.h>

#include "LR35902.h"
#include "memory.h"

LR35902::InstrFunc LR35902::optable[LR35902::table_size];
LR35902::OpInfo LR35902::infotable[LR35902::table_size];
const LR35902::OpInfo LR35902::unknown_info;
const LR35902::Instruction LR35902::implemented_instruction_table[];

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
  reg.hl = memory.get8(reg.pc + 1);
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
  reg.a = reg.hl;
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
  reg.b = reg.hl;
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
  reg.c = reg.hl;
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
  reg.d = reg.hl;
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
  reg.e = reg.hl;
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
  reg.h = reg.hl;
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
  reg.l = reg.hl;
}

void LR35902::LD_hl_b()
{
  reg.hl = reg.b;
}

void LR35902::LD_hl_c()
{
  reg.hl = reg.c;
}

void LR35902::LD_hl_d()
{
  reg.hl = reg.d;
}

void LR35902::LD_hl_e()
{
  reg.hl = reg.e;
}

void LR35902::LD_hl_h()
{
  reg.hl = reg.h;
}

void LR35902::LD_hl_l()
{
  reg.hl = reg.l;
}
