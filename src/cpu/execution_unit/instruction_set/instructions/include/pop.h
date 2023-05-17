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
  Pop(Registers *registers, BUS *bus, StackStrategy const *stack_stragegy)
      : MicroOp(bus, registers), _stack_strategy(stack_stragegy) {}

public:
  StackStrategy const *_stack_strategy;
};

struct PopRegister : Pop {
  explicit PopRegister(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopRegister(BUS *bus, Registers *registers,
              StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void after_execute(UNUSED_PARAM const Instruction &) override {
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PopRegister, WordMovOpTypeSelector, MovOperator,
                           STKR_Decoder)
};

struct PopMemory : Pop {
  explicit PopMemory(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopMemory(BUS *bus, Registers *registers, StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void after_execute(UNUSED_PARAM const Instruction &) override {
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PopMemory, WordMovOpTypeSelector, MovOperator,
                           STKM_Decoder)
};

struct PopSegment : Pop {
  explicit PopSegment(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopSegment(BUS *bus, Registers *registers,
             StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void after_execute(UNUSED_PARAM const Instruction &) override {
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PopSegment, WordMovOpTypeSelector, MovOperator,
                           STKS_Decoder)
};

struct PopFlags : Pop {
  explicit PopFlags(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopFlags(BUS *bus, Registers *registers, StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void after_execute(UNUSED_PARAM const Instruction &) override {
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PopFlags, WordMovOpTypeSelector, MovOperator,
                           STKF_Decoder)
};

#endif // _POP_H_
