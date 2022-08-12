//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _XCHG_H_
#define _XCHG_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "types.h"
#include "xchg_operators.h"

struct XCHG : public MicroOp {
  XCHG(BUS *bus, Registers *registers) : _registers(registers), _bus(bus) {}

protected:
  Registers *_registers;
  BUS *_bus;
};

struct XCHGAccumulator : public XCHG {
  XCHGAccumulator(BUS *bus, Registers *registers) : XCHG(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(XCHGAccumulator, WordOpTypeSelector, XCHGOperator,
                           AXRXCH_Decoder)
};

struct XCHGRegisterMemory : public XCHG {
  XCHGRegisterMemory(BUS *bus, Registers *registers) : XCHG(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(XCHGRegisterMemory, WordOrByteOpcodeOpTypeSelector,
                           XCHGOperator, RMXCH_Decoder)
};

#endif // _XCHG_H_