//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _HALT_H_
#define _HALT_H_

#include "logger.h"
#include "micro_op.h"
#include "instruction_templates.h"

struct HALT : MicroOp {
  HALT(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override {
    _registers->FLAGS.to_bits<flags_t>()->H = 1;
  }

  MICRO_OP_INSTRUCTION(HALT)
};

#endif // _HALT_H_
