//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _LEA_H_
#define _LEA_H_

#include "decoders.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct LEA : MicroOp {
  LEA(bus_ptr_t bus, registers_ptr_t registers)
      : _bus(bus), _registers(registers) {}

  MICRO_OP_INSTRUCTION_DCR(LEA, WordMovOpTypeSelector, MovOperator, REA_Decoder)

protected:
  bus_ptr_t _bus;
  registers_ptr_t _registers;
};

#endif // _LEA_H_