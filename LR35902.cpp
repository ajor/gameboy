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
}

void LR35902::LD_a_b()
{
}

void LR35902::LD_a_c()
{
}

void LR35902::LD_a_d()
{
}

void LR35902::LD_a_e()
{
}

void LR35902::LD_a_h()
{
}

void LR35902::LD_a_l()
{
}

void LR35902::LD_a_hl()
{
}

void LR35902::LD_b_b()
{
}

void LR35902::LD_b_c()
{
}

void LR35902::LD_b_d()
{
}

void LR35902::LD_b_e()
{
}

void LR35902::LD_b_h()
{
}

void LR35902::LD_b_l()
{
}

void LR35902::LD_b_hl()
{
}

void LR35902::LD_c_b()
{
}

void LR35902::LD_c_c()
{
}

void LR35902::LD_c_d()
{
}

void LR35902::LD_c_e()
{
}

void LR35902::LD_c_h()
{
}

void LR35902::LD_c_l()
{
}

void LR35902::LD_c_hl()
{
}

void LR35902::LD_d_b()
{
}

void LR35902::LD_d_c()
{
}

void LR35902::LD_d_d()
{
}

void LR35902::LD_d_e()
{
}

void LR35902::LD_d_h()
{
}

void LR35902::LD_d_l()
{
}

void LR35902::LD_d_hl()
{
}

void LR35902::LD_e_b()
{
}

void LR35902::LD_e_c()
{
}

void LR35902::LD_e_d()
{
}

void LR35902::LD_e_e()
{
}

void LR35902::LD_e_h()
{
}

void LR35902::LD_e_l()
{
}

void LR35902::LD_e_hl()
{
}

void LR35902::LD_h_b()
{
}

void LR35902::LD_h_c()
{
}

void LR35902::LD_h_d()
{
}

void LR35902::LD_h_e()
{
}

void LR35902::LD_h_h()
{
}

void LR35902::LD_h_l()
{
}

void LR35902::LD_h_hl()
{
}

void LR35902::LD_l_b()
{
}

void LR35902::LD_l_c()
{
}

void LR35902::LD_l_d()
{
}

void LR35902::LD_l_e()
{
}

void LR35902::LD_l_h()
{
}

void LR35902::LD_l_l()
{
}

void LR35902::LD_l_hl()
{
}

void LR35902::LD_hl_b()
{
}

void LR35902::LD_hl_c()
{
}

void LR35902::LD_hl_d()
{
}

void LR35902::LD_hl_e()
{
}

void LR35902::LD_hl_h()
{
}

void LR35902::LD_hl_l()
{
}
