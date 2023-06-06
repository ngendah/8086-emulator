//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _DAS_H_
#define _DAS_H_

#include "logger.h"
#include "micro_op.h"

struct DAS : MicroOp {
  DAS(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(DAS)
};

#endif // _DAS_H_
