//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _SHR_H
#define _SHR_H

#include "logger.h"
#include "micro_op.h"

struct SHR : MicroOp {
  SHR(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(SHR)
};

#endif // _SHR_H
