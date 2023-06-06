//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _SCAS_H_
#define _SCAS_H_

#include "logger.h"
#include "micro_op.h"

struct SCAS : MicroOp {
  SCAS(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(UNUSED_PARAM const Instruction &) override { assert(false); }

  MICRO_OP_INSTRUCTION(SCAS)
};

#endif // _SCAS_H_
