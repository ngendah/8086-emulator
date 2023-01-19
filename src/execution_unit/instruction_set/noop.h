//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _NOOP_H_
#define _NOOP_H_

#include "logger.h"
#include "types.h"

struct NoOp : MicroOp {
  NoOp(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override {}

  MICRO_OP_INSTRUCTION(NoOp)
};

#define NOP NoOp

#endif // _NOOP_H_
