//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INCR_H_
#define _INCR_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "math_operators.h"
#include "micro_op.h"
#include "mov_operators.h"
#include "types.h"

struct INCRRegister : MicroOp {
  INCRRegister(bus_ptr_t bus, registers_ptr_t registers)
      : MicroOp(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCRE(INCRRegister, WordMovOpTypeSelector, MathOperator,
                            IncrOpType, DRR_Decoder)

  void after_execute(UNUSED_PARAM const Instruction &) override {
    PLOGD << _registers->FLAGS.bits<flags_t>();
  }
};

#endif // _INCR_H_