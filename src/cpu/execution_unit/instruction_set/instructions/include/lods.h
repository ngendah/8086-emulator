//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _LODS_H_
#define _LODS_H_

#include "logger.h"
#include "micro_op.h"

struct LODS : MicroOp {
  LODS(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(LODS)
};

#endif // _LODS_H_
