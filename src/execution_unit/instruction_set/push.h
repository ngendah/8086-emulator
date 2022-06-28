#ifndef _PUSH_H_
#define _PUSH_H_

#include "instruction_templates.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

class Push {
public:
  Push(Registers *registers, BUS *bus) : _registers(registers), _bus(bus) {}

  virtual void execute(const Instruction &instruction) = 0;

protected:
  Registers *_registers;
  BUS *_bus;

  struct _IOWriter final : IOWriter {
    StackMemoryIOSelector _stack_mem_selector;
    explicit _IOWriter(BUS *bus, Registers *registers)
        : _stack_mem_selector(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      return _stack_mem_selector.get(instruction);
    }
  };
};

class PushRegister : public Push {
public:
  PushRegister(Registers *registers, BUS *bus) : Push(registers, bus) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.mov(instruction);
    _registers->SP += sizeof(uint16_t);
  }

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
  PushMemory(Registers *registers, BUS *bus) : Push(registers, bus) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_bus, _registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.mov(instruction);
    _registers->SP += sizeof(uint16_t);
  }

protected:
  struct _IOReader final : IOReader {
    MemoryIOSelector _mem_selector;
    explicit _IOReader(BUS *bus, Registers *registers)
        : _mem_selector(bus, registers) {}

    IO *reader(const Instruction &instruction) override {
      auto mode = instruction.mode_to<mod_reg_rm_t>();
      assert(mode.REG == 0x6);
      return _mem_selector.get(instruction);
    }
  };
};

class PushSegment : public Push {
public:
  PushSegment(Registers *registers, BUS *bus) : Push(registers, bus) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.mov(instruction);
    _registers->SP += sizeof(uint16_t);
  }

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