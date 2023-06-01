//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _POP_H_
#define _POP_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "micro_op.h"
#include "mov_operators.h"
#include "stack_strategy.h"
#include "types.h"

struct Pop : MicroOp {
  Pop(Registers *registers, BUS *bus) : MicroOp(bus, registers) {}

  template <class TStackStrategy> struct PopOpType : OpType {
    void execute(const OpType::Params &params) const override {
      if (params._destination == nullptr || params._source == nullptr) {
        PLOGD << "source or destination is not available, is null";
        return;
      }
      TStackStrategy::prev(params);
    }
  };
};

struct PopRegister : Pop {
  PopRegister(BUS *bus, Registers *registers) : Pop(registers, bus) {}
#ifndef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PopRegister, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullDescending>, STKR_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PopRegister, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullAscending>, STKR_Decoder)
#endif
};

struct PopMemory : Pop {
  PopMemory(BUS *bus, Registers *registers) : Pop(registers, bus) {}
#ifndef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PopMemory, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullDescending>, STKM_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PopMemory, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullAscending>, STKM_Decoder)
#endif
};

struct PopSegment : Pop {
  PopSegment(BUS *bus, Registers *registers) : Pop(registers, bus) {}
#ifndef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PopSegment, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullDescending>, STKS_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PopSegment, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullAscending>, STKS_Decoder)
#endif
};

struct PopFlags : Pop {
  PopFlags(BUS *bus, Registers *registers) : Pop(registers, bus) {}
#ifndef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PopFlags, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullDescending>, STKF_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PopFlags, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullAscending>, STKF_Decoder)
#endif
};

struct PopIP : Pop {
  PopIP(BUS *bus, Registers *registers) : Pop(registers, bus) {}
#ifndef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PopIP, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullDescending>, STKIP_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PopIP, WordMovOpTypeSelector, MovOperator,
                            PopOpType<StackFullAscending>, STKIP_Decoder)
#endif
};

#endif // _POP_H_
