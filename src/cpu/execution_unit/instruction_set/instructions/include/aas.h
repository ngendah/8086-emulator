//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _AAS_H_
#define _AAS_H_

#include "logger.h"
#include "micro_op.h"

struct AAS : MicroOp {
  AAS(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(AAS)
};

#endif // _AAS_H_
