//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _PUSH_H_
#define _PUSH_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "micro_op.h"
#include "mov_operators.h"
#include "stack_strategy.h"
#include "types.h"

struct Push : MicroOp {
  Push(Registers *registers, BUS *bus) : MicroOp(bus, registers) {}

  template <class TStackStrategy> struct PushOpType : OpType {
    void execute(const OpType::Params &params) const override {
      if (params._destination == nullptr || params._source == nullptr) {
        PLOGD << "source or destination is null";
        return;
      }
      TStackStrategy::push(params);
    }
  };
};

struct PushRegister : Push {
  PushRegister(BUS *bus, Registers *registers) : Push(registers, bus) {}
#ifdef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PushRegister, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullAscending>, RSTK_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PushRegister, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullDescending>, RSTK_Decoder)
#endif
};

struct PushMemory : Push {
  PushMemory(BUS *bus, Registers *registers) : Push(registers, bus) {}
#ifdef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PushMemory, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullAscending>, MSTK_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PushMemory, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullDescending>, MSTK_Decoder)
#endif
};

struct PushSegment : Push {
  PushSegment(BUS *bus, Registers *registers) : Push(registers, bus) {}
#ifdef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PushSegment, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullAscending>, SSTK_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PushSegment, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullDescending>, SSTK_Decoder)
#endif
};

struct PushFlags : Push {
  PushFlags(BUS *bus, Registers *registers) : Push(registers, bus) {}
#ifdef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PushFlags, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullAscending>, FSTK_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PushFlags, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullDescending>, FSTK_Decoder)
#endif
};

struct PushIP : Push {
  PushIP(BUS *bus, Registers *registers) : Push(registers, bus) {}
#ifdef _STACK_FULL_ASCENDING
  MICRO_OP_INSTRUCTION_DCRE(PushIP, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullAscending>, IPSTK_Decoder)
#else
  MICRO_OP_INSTRUCTION_DCRE(PushIP, WordMovOpTypeSelector, MovOperator,
                            PushOpType<StackFullDescending>, IPSTK_Decoder)
#endif
};

#endif // _PUSH_H_
