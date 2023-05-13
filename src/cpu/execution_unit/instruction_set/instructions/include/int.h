//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INT_H_
#define _INT_H_

#include "logger.h"
#include "micro_op.h"

struct INT : MicroOp {
  INT(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(INT)
};

#endif // _INT_H_
