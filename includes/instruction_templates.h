#ifndef INSTRUCTION_TEMPLATES_H_
#define INSTRUCTION_TEMPLATES_H_

#include "types.h"

namespace InstructionTemplates{
  typedef InstructionTemplate<opcode_d_w_t, mod_reg_rm_t> Template1;
  typedef InstructionTemplate<opcode_w_t, mod_rm_t> Template2;

};

#endif // INSTRUCTION_TEMPLATES_H_
