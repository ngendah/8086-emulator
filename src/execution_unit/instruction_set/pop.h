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

class Pop : public MicroOp {
public:
  Pop(Registers *registers, BUS *bus, StackStrategy const *stack_stragegy)
      : _registers(registers), _bus(bus), _stack_strategy(stack_stragegy) {}

  virtual void execute(const Instruction &instruction) = 0;

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

class PopRegister : public Pop {
public:
  explicit PopRegister(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopRegister(BUS *bus, Registers *registers,
              StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_bus, _registers);
    auto io_writer = _IOWriter(_registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION(PopRegister)

protected:
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      auto mode = instruction.opcode_to<opcode_reg_t>();
      return mode.REG;
    }
  };

  struct _IOWriter final : IOWriter {
    Registers *_registers;
    _IOWriter(Registers *registers) : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      auto reg_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &reg_selector).get(instruction);
    }
  };
};

class PopMemory : public Pop {
public:
  explicit PopMemory(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopMemory(BUS *bus, Registers *registers, StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_bus, _registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION(PopMemory)

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

class PopSegment : public Pop {
public:
  explicit PopSegment(BUS *bus, Registers *registers)
      : Pop(registers, bus, &stack_full_descending) {}

  PopSegment(BUS *bus, Registers *registers,
             StackStrategy const *stack_stragegy)
      : Pop(registers, bus, stack_stragegy) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_bus, _registers);
    auto io_writer = _IOWriter(_registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
    _stack_strategy->prev(_registers->SP, sizeof(uint16_t));
  }

  MICRO_OP_INSTRUCTION(PopSegment)

protected:
  struct _IOWriter final : IOWriter {
    Registers *_registers;
    explicit _IOWriter(Registers *registers) : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      auto seg_selector = OpCodeSegmentSelector();
      return SegmentIOSelector(_registers, &seg_selector).get(instruction);
    }
  };
};

// TODO pop flags

#endif // _POP_H_