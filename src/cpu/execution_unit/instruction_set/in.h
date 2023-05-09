//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _IN_H_
#define _IN_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "micro_op.h"
#include "mov_operators.h"
#include "types.h"

struct IN : MicroOp {
  IN(Registers *registers, BUS *bus) : MicroOp(bus, registers) {}
};

struct INPort : IN {
  INPort(BUS *bus, Registers *registers) : IN(registers, bus) {}

  MICRO_OP_INSTRUCTION_DCR(INPort, WordOrByteMovOpTypeSelector, MovOperator,
                           IAX_Decoder)
};

struct INDX : IN {
  INDX(BUS *bus, Registers *registers) : IN(registers, bus) {}

  MICRO_OP_INSTRUCTION_DCR(INDX, WordOrByteMovOpTypeSelector, MovOperator,
                           IAXDX_Decoder)
};

#endif // PORT_ADDRESSING