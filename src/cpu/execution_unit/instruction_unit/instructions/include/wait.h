//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _WAIT_H_
#define _WAIT_H_

#include "logger.h"
#include "micro_op.h"

struct WAIT : MicroOp {
  WAIT(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(WAIT)
};

#endif // _WAIT_H_
