//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _DEC_H_
#define _DEC_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "math_operators.h"
#include "micro_op.h"
#include "mov_operators.h"
#include "types.h"

struct DECRegister : MicroOp {
  DECRegister(bus_ptr_t bus, registers_ptr_t registers)
      : MicroOp(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCRE(DECRegister, WordMovOpTypeSelector, MathOperator,
                            DecrOpType, DRR_Decoder)

  void after_execute(UNUSED_PARAM const Instruction &) override {
    PLOGD << _registers->FLAGS.bits<flags_t>();
  }
};

struct DECMemory : MicroOp {
  DECMemory(bus_ptr_t bus, registers_ptr_t registers)
      : MicroOp(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCRE(DECMemory, WordMovOpTypeSelector, MathOperator,
                            DecrOpType, DMM_Decoder)

  void after_execute(UNUSED_PARAM const Instruction &) override {
    PLOGD << _registers->FLAGS.bits<flags_t>();
  }
};

struct DECRegisterMemory : MicroOp {
  DECRegisterMemory(bus_ptr_t bus, registers_ptr_t registers)
      : MicroOp(bus, registers) {}

  void execute(const Instruction &instruction) {
    auto mod_rm = instruction.mode_to<mod_rm_t>();
    if (mod_rm.RM == AddressingModes::MOD::REG) {
      opcode_reg_t opcode_reg = {};
      opcode_reg.OPCODE = 0x9;
      opcode_reg.REG = mod_rm.RM;
      DECRegister(_bus, _registers)
          .execute(Instruction(0xff, make_word((uint8_t)opcode_reg, 0)));
    } else {
      DECMemory(_bus, _registers).execute(instruction);
    }
  }

  MICRO_OP_INSTRUCTION(DECRegisterMemory)
};

#endif // _DEC_H_