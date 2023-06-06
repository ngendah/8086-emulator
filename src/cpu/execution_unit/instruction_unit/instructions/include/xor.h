//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _XOR_H_
#define _XOR_H_

#include "logger.h"
#include "micro_op.h"

struct XOR : MicroOp {
  XOR(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(XOR)
};

#endif // _XOR_H_
