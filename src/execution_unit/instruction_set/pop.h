//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _POP_H_
#define _POP_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "stack_strategy.h"
#include "types.h"

struct Pop : public MicroOp {
  Pop(Registers *registers, BUS *bus, StackStrategy const *stack_stragegy)
      : _registers(registers), _bus(bus), _stack_strategy(stack_stragegy) {}

protected:
  Registers *_registers;
  BUS *_bus;

public:
  StackStrategy const *_stack_strategy;
};

struct PopRegister : public Pop {
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

struct PopMemory : public Pop {
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

struct PopSegment : public Pop {
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

// TODO pop flags

#endif // _POP_H_