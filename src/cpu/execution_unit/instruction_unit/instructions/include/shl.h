//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _SHL_H
#define _SHL_H

#include "logger.h"
#include "micro_op.h"

struct SHL : MicroOp {
  SHL(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(SHL)
};

#endif // _SHL_H
