//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _POP_H_
#define _POP_H_

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

  struct _IOReader final : IOReader {
    StackMemoryIOSelector _stack_mem_selector;

    _IOReader(BUS *bus, Registers *registers)
        : _stack_mem_selector(bus, registers) {}

    IO *reader(const Instruction &instruction) override {
      return _stack_mem_selector.get(instruction);
    }
  };

public:
  StackStrategy const *_stack_strategy;
};

struct PopRegister : public Pop {
  explicit PopRegister(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopRegister(BUS *bus, Registers *registers,
              StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void after_execute(__attribute__((unused)) const Instruction &) override {
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_OVR(PopRegister, WordMovOpTypeSelector, MovOperator)

protected:
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      auto mode = instruction.opcode_to<opcode_reg_t>();
      return mode.REG;
    }
  };

  struct _IOWriter final : IOWriter {
    Registers *_registers;
    _IOWriter(BUS *bus, Registers *registers) : _registers(registers) {
      UNUSED(bus);
    }

    IO *writer(const Instruction &instruction) override {
      auto reg_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &reg_selector).get(instruction);
    }
  };
};

struct PopMemory : public Pop {
  explicit PopMemory(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopMemory(BUS *bus, Registers *registers, StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void after_execute(__attribute__((unused)) const Instruction &) override {
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_OVR(PopMemory, WordMovOpTypeSelector, MovOperator)

protected:
  struct _IOWriter final : IOWriter {
    MemoryIOSelector _mem_selector;
    _IOWriter(BUS *bus, Registers *registers) : _mem_selector(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      auto mode = instruction.mode_to<mod_reg_rm_t>();
      assert(mode.REG == 0x0);
      return _mem_selector.get(instruction);
    }
  };
};

struct PopSegment : public Pop {
  explicit PopSegment(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopSegment(BUS *bus, Registers *registers,
             StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void after_execute(__attribute__((unused)) const Instruction &) override {
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION_OVR(PopSegment, WordMovOpTypeSelector, MovOperator)

protected:
  struct _IOWriter final : IOWriter {
    Registers *_registers;

    explicit _IOWriter(BUS *bus, Registers *registers) : _registers(registers) {
      UNUSED(bus);
    }

    IO *writer(const Instruction &instruction) override {
      auto seg_selector = OpCodeSegmentSelector();
      return SegmentIOSelector(_registers, &seg_selector).get(instruction);
    }
  };
};

// TODO pop flags

#endif // _POP_H_