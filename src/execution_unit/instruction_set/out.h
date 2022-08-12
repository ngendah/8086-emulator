//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _OUT_H_
#define _OUT_H_

#include "decoders.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct OUT : public MicroOp {
  OUT(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}
};

struct OUTPort : public OUT {
  OUTPort(bus_ptr_t bus, registers_ptr_t registers) : OUT(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(OUTPort, WordOrByteMovOpTypeSelector, MovOperator,
                           AXO_Decoder)
};

struct OUTDX : public OUT {
  OUTDX(bus_ptr_t bus, registers_ptr_t registers) : OUT(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(OUTDX, WordOrByteMovOpTypeSelector, MovOperator,
                           AXDX_Decoder)
};

#endif // _OUT_H_