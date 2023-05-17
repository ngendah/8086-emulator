//
// Copyright (c) 2022 Ngenda Henry
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
  Push(Registers *registers, BUS *bus, StackStrategy const *stack_stragegy)
      : MicroOp(bus, registers), _stack_strategy(stack_stragegy) {}

protected:
  StackStrategy const *_stack_strategy;
};

struct PushRegister : Push {
  explicit PushRegister(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushRegister(BUS *bus, Registers *registers,
               StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void after_execute(UNUSED_PARAM const Instruction &) override {
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PushRegister, WordMovOpTypeSelector, MovOperator,
                           RSTK_Decoder)
};

struct PushMemory : Push {
  explicit PushMemory(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushMemory(BUS *bus, Registers *registers,
             StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void after_execute(UNUSED_PARAM const Instruction &) override {
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PushMemory, WordMovOpTypeSelector, MovOperator,
                           MSTK_Decoder)
};

struct PushSegment : Push {
  explicit PushSegment(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushSegment(BUS *bus, Registers *registers,
              StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void after_execute(UNUSED_PARAM const Instruction &) override {
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PushSegment, WordMovOpTypeSelector, MovOperator,
                           SSTK_Decoder)
};

struct PushFlags : Push {
  explicit PushFlags(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushFlags(BUS *bus, Registers *registers, StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void after_execute(UNUSED_PARAM const Instruction &) override {
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PushFlags, WordMovOpTypeSelector, MovOperator,
                           FSTK_Decoder)
};

#endif // _PUSH_H_
