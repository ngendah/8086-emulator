//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _XLAT_H_
#define _XLAT_H_

#include "decoders.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct XLAT : MicroOp {
  XLAT(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  Instruction before_decode(UNUSED_PARAM const Instruction &) override {
    return Instruction(SegmentMapper::SOP_DS_INDEX, 0x0);
  }

  MICRO_OP_INSTRUCTION_DCR(XLAT, WordOrByteMovOpTypeSelector, MovOperator,
                           AXMXLAT_Decoder)
};

#endif // _XLAT_H_