//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _CMP_H_
#define _CMP_H_

#include "address.h"
#include "decoders.h"
#include "math_operators.h"
#include "micro_op.h"
#include "mov_operators.h"
#include "types.h"

// TODO refactoring opportunities
// very similar to mov instructions

struct CMP : MicroOp {
  CMP(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}
};

struct CMPRegisters : CMP {
  CMPRegisters(bus_ptr_t bus, registers_ptr_t registers)
      : CMP(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCRE(CMPRegisters, WordOrByteMovOpTypeSelector,
                            MathOperator, CompareOpType, RR_Decoder)
};

struct CMPRegisterMemory : CMP {
  CMPRegisterMemory(bus_ptr_t bus, registers_ptr_t registers)
      : CMP(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCRE(CMPRegisterMemory, WordOrByteMovOpTypeSelector,
                            MathOperator, CompareOpType, RM_Decoder)
};

struct CMPRegisterAndMemory : MicroOp {
  CMPRegisterAndMemory(bus_ptr_t bus, registers_ptr_t registers)
      : MicroOp(bus, registers) {}

  void execute(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    auto params = MicroOp::Params(_bus, _registers);
    if (mode.MOD == AddressingModes::MOD::REG) {
      return CMPRegisters::create(params)->execute(instruction);
    }
    return CMPRegisterMemory::create(params)->execute(instruction);
  }

  MICRO_OP_INSTRUCTION(CMPRegisterAndMemory)
};

struct CMPRegisterImmediate : CMP {
  CMPRegisterImmediate(bus_ptr_t bus, registers_ptr_t registers)
      : CMP(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCRE(CMPRegisterImmediate,
                            WordOrByteWithSignOpTypeSelector, MathOperator,
                            CompareOpType, CMP_RI_Decoder)
};

struct CMPAXImmediate : CMP {
  CMPAXImmediate(bus_ptr_t bus, registers_ptr_t registers)
      : CMP(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCRE(CMPAXImmediate, WordOrByteMovOpTypeSelector,
                            MathOperator, CompareOpType, CMP_AXI_Decoder)
};

#endif // _CMP_H_
