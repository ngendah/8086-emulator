//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _CMPS_H_
#define _CMPS_H_

#include "logger.h"
#include "micro_op.h"

struct CMPS : MicroOp {
  CMPS(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(CMPS)
};

#endif // _CMPS_H_
