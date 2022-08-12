//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _PUSH_H_
#define _PUSH_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "stack_strategy.h"
#include "types.h"

struct Push : public MicroOp {
  Push(Registers *registers, BUS *bus, StackStrategy const *stack_stragegy)
      : _registers(registers), _bus(bus), _stack_strategy(stack_stragegy) {}

protected:
  Registers *_registers;
  BUS *_bus;

public:
  StackStrategy const *_stack_strategy;
};

struct PushRegister : public Push {
  explicit PushRegister(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushRegister(BUS *bus, Registers *registers,
               StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void after_execute(const Instruction &instruction) override {
    Push::after_execute(instruction);
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PushRegister, WordMovOpTypeSelector, MovOperator,
                           RSTK_Decoder)
};

struct PushMemory : public Push {
  explicit PushMemory(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushMemory(BUS *bus, Registers *registers,
             StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void after_execute(const Instruction &instruction) override {
    Push::after_execute(instruction);
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PushMemory, WordMovOpTypeSelector, MovOperator,
                           MSTK_Decoder)
};

struct PushSegment : public Push {
  explicit PushSegment(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushSegment(BUS *bus, Registers *registers,
              StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void after_execute(const Instruction &instruction) override {
    Push::after_execute(instruction);
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_DCR(PushSegment, WordMovOpTypeSelector, MovOperator,
                           SSTK_Decoder)
};
#endif // _PUSH_H_