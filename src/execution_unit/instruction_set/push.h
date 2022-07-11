//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _PUSH_H_
#define _PUSH_H_

#include "instruction_templates.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "stack_strategy.h"
#include "types.h"

class Push : public MicroOp {
public:
  Push(Registers *registers, BUS *bus, StackStrategy const *stack_stragegy)
      : _registers(registers), _bus(bus), _stack_strategy(stack_stragegy) {}

protected:
  Registers *_registers;
  BUS *_bus;

  struct _IOWriter final : IOWriter {
    StackMemoryIOSelector _stack_mem_selector;
    _IOWriter(BUS *bus, Registers *registers)
        : _stack_mem_selector(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      return _stack_mem_selector.get(instruction);
    }
  };

public:
  StackStrategy const *_stack_strategy;
};

class PushRegister : public Push {
public:
  explicit PushRegister(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushRegister(BUS *bus, Registers *registers,
               StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION(PushRegister)
protected:
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      auto mode = instruction.opcode_to<opcode_reg_t>();
      return mode.REG;
    }
  };

  struct _IOReader final : IOReader {
    Registers *_registers;
    explicit _IOReader(Registers *registers) : _registers(registers) {}

    IO *reader(const Instruction &instruction) override {
      auto reg_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &reg_selector).get(instruction);
    }
  };
};

class PushMemory : public Push {
public:
  explicit PushMemory(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushMemory(BUS *bus, Registers *registers,
             StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_bus, _registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION(PushMemory)

protected:
  struct _IOReader final : IOReader {
    MemoryIOSelector _mem_selector;

    _IOReader(BUS *bus, Registers *registers) : _mem_selector(bus, registers) {}

    IO *reader(const Instruction &instruction) override {
      auto mode = instruction.mode_to<mod_reg_rm_t>();
      assert(mode.REG == 0x6);
      return _mem_selector.get(instruction);
    }
  };
};

class PushSegment : public Push {
public:
  explicit PushSegment(BUS *bus, Registers *registers)
      : Push(registers, bus, &stack_full_descending) {}

  PushSegment(BUS *bus, Registers *registers,
              StackStrategy const *stack_stragegy)
      : Push(registers, bus, stack_stragegy) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
    _stack_strategy->next(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION(PushSegment)

protected:
  struct _IOReader final : IOReader {
    Registers *_registers;
    explicit _IOReader(Registers *registers) : _registers(registers) {}

    IO *reader(const Instruction &instruction) override {
      auto seg_selector = OpCodeSegmentSelector();
      return SegmentIOSelector(_registers, &seg_selector).get(instruction);
    }
  };
};
#endif // _PUSH_H_