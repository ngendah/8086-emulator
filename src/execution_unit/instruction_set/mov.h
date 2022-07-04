//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _MOV_H_
#define _MOV_H_

#include "instruction_templates.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

class MovRegisterRegister {
public:
  explicit MovRegisterRegister(Registers *registers) : _registers(registers) {}

  void execute(const Instruction &instruction) {
    auto mod = instruction.mode_to<mod_reg_rm_t>();
    PLOGD << instruction;
    PLOGD << mod;
    assert(mod.MOD == 0x3);
    return mov(instruction);
  }

protected:
  Registers *_registers;

  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct _RegisterSelector2 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().RM;
    }
  };

  struct _IOReader final : IOReader, _RegisterSelector1, _RegisterSelector2 {
    Registers *_registers;
    std::array<RegisterSelector *, 2> _selectors;

    explicit _IOReader(Registers *registers) : _registers(registers) {
      _selectors = {
          (_RegisterSelector1 *)this,
          (_RegisterSelector2 *)this,
      };
    }

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return RegisterIOSelector(_registers, _selectors[opcode.D])
          .get(instruction);
    }
  };

  struct _IOWriter final : IOWriter, _RegisterSelector1, _RegisterSelector2 {
    Registers *_registers;
    std::array<RegisterSelector *, 2> _selectors;

    explicit _IOWriter(Registers *registers) : _registers(registers) {
      _selectors = {
          (_RegisterSelector2 *)this,
          (_RegisterSelector1 *)this,
      };
    }

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return RegisterIOSelector(_registers, _selectors[opcode.D])
          .get(instruction);
    }
  };

  void mov(const Instruction &instruction) {
    auto op_selector = RegisterMovOpTypeSelector();
    auto mov_operator =
        MovOperator(_IOReader(_registers).reader(instruction),
                    _IOWriter(_registers).writer(instruction), &op_selector);
    mov_operator.execute(instruction);
  }
};

class MovRegisterImmediate {
public:
  explicit MovRegisterImmediate(Registers *registers) : _registers(registers) {}

  void execute(const Instruction &instruction) {
    auto op_selector = ImmediateMovOpTypeSelector();
    return MovOperator(_IOReader().reader(instruction),
                       _IOWriter(_registers).writer(instruction), &op_selector)
        .execute(instruction);
  }

protected:
  Registers *_registers;

  struct _RegisterSelector : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.opcode_to<opcode_w_reg_t>().REG;
    }
  };

  struct _IOReader : IOReader {
    ValueIO _value_io;
    explicit _IOReader() = default;

    IO *reader(const Instruction &instruction) override {
      auto w_reg = instruction.opcode_to<opcode_w_reg_t>();
      if (w_reg.W == 1)
        _value_io = instruction.data<uint16_t>();
      else
        _value_io = instruction.data<uint8_t>();
      return &_value_io;
    }
  };

  struct _IOWriter : IOWriter, _RegisterSelector {
    Registers *_registers;

    explicit _IOWriter(Registers *registers) : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      return RegisterIOSelector(_registers, this).get(instruction);
    }
  };
};

class MovRegisterMemory {
public:
  explicit MovRegisterMemory(Registers *registers, BUS *bus)
      : _registers(registers), _bus(bus) {}

  virtual void execute(const Instruction &instruction) {
    auto opcode = instruction.opcode_to<d_w_t>();
    assert(opcode.W == 1);
    auto op_selector = WordMovOpTypeSelector();
    auto io_reader = _IOReader(_bus, _registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
  }

protected:
  Registers *_registers;
  BUS *_bus;

  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct _RWIO {
    Registers *_registers;
    MemoryIOSelector _io_selector;

    explicit _RWIO(BUS *bus, Registers *registers)
        : _registers(registers), _io_selector(bus, registers) {}

    IO *register_selector(const Instruction &instruction) {
      auto register_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }

    IO *memory_selector(const Instruction &instruction) {
      return _io_selector.get(instruction);
    }
  };

  struct _IOReader final : _RWIO, IOReader {
    explicit _IOReader(BUS *bus, Registers *registers)
        : _RWIO(bus, registers) {}

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      assert(opcode.D == 0);
      return opcode.D == 1 ? memory_selector(instruction)
                           : register_selector(instruction);
    }
  };

  struct _IOWriter final : _RWIO, IOWriter {

    explicit _IOWriter(BUS *bus, Registers *registers)
        : _RWIO(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? register_selector(instruction)
                           : memory_selector(instruction);
    }
  };
};

class MovRegisterSegment {
public:
  explicit MovRegisterSegment(Registers *registers) : _registers(registers) {}

  void execute(const Instruction &instruction) {
    PLOGD << instruction;
    auto ins_temp = InstructionTemplate<d_w_t, mod_sr_rm_t>(instruction);
    assert(ins_temp.opcode().W == 0);
    assert(ins_temp.mode().MOD == 0x3);
    auto op_selector = WordMovOpTypeSelector();
    auto mov_operator =
        MovOperator(_IOReader(_registers).reader(instruction),
                    _IOWriter(_registers).writer(instruction), &op_selector);
    mov_operator.execute(instruction);
  }

protected:
  Registers *_registers;

  struct _RegisterSelector : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_sr_rm_t>().RM;
    }
  };

  struct _RWIO {
    Registers *_registers;

    explicit _RWIO(Registers *registers) : _registers(registers) {}

    IO *register_selector(const Instruction &instruction) {
      auto register_selector = _RegisterSelector();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }

    IO *segment_selector(const Instruction &instruction) {
      return SegmentIOSelector(_registers).get(instruction);
    }
  };

  struct _IOReader final : _RWIO, IOReader {
    explicit _IOReader(Registers *registers) : _RWIO(registers) {}

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? register_selector(instruction)
                           : segment_selector(instruction);
    }
  };

  struct _IOWriter final : _RWIO, IOWriter {
    explicit _IOWriter(Registers *registers) : _RWIO(registers) {}

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? segment_selector(instruction)
                           : register_selector(instruction);
    }
  };
};

class MovMemorySegment {
public:
  explicit MovMemorySegment(BUS *bus, Registers *registers)
      : _registers(registers), _bus(bus) {}

  void execute(const Instruction &instruction) {
    auto op_selector = WordMovOpTypeSelector();
    auto opcode = instruction.opcode_to<d_w_t>();
    PLOGD << opcode;
    assert(opcode.W == 0);
    auto io_reader = _IOReader(_bus, _registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
  }

protected:
  Registers *_registers;
  BUS *_bus;

  struct _RWIO {
    Registers *_registers;
    MemoryIOSelector _io_selector;

    explicit _RWIO(BUS *bus, Registers *registers)
        : _registers(registers), _io_selector(bus, registers) {}

    IO *segment_selector(const Instruction &instruction) {
      return SegmentIOSelector(_registers).get(instruction);
    }

    IO *memory_selector(const Instruction &instruction) {
      return _io_selector.get(instruction);
    }
  };

  struct _IOReader final : _RWIO, IOReader {
    explicit _IOReader(BUS *bus, Registers *registers)
        : _RWIO(bus, registers) {}

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? memory_selector(instruction)
                           : segment_selector(instruction);
    }
  };

  struct _IOWriter final : _RWIO, IOWriter {
    explicit _IOWriter(BUS *bus, Registers *registers)
        : _RWIO(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? segment_selector(instruction)
                           : memory_selector(instruction);
    }
  };
};

class MovAccumulator {
public:
  explicit MovAccumulator(Registers *registers, BUS *bus)
      : _registers(registers), _bus(bus) {}

  void execute(const Instruction &instruction) {
    auto op_selector = WordOrByteMovOpTypeSelector();
    auto io_reader = _IOReader(_bus, _registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
  }

protected:
  Registers *_registers;
  BUS *_bus;

  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(__attribute__((unused)) const Instruction &) const {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct _RWIO {
    Registers *_registers;
    DirectMemoryIOSelector _io_selector;

    explicit _RWIO(BUS *bus, Registers *registers)
        : _registers(registers), _io_selector(bus, registers) {}

    IO *register_selector(const Instruction &instruction) {
      auto register_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }

    IO *memory_selector(const Instruction &instruction) {
      return _io_selector.get(instruction);
    }
  };

  struct _IOReader final : _RWIO, IOReader {
    explicit _IOReader(BUS *bus, Registers *registers)
        : _RWIO(bus, registers) {}

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? register_selector(instruction)
                           : memory_selector(instruction);
    }
  };

  struct _IOWriter final : _RWIO, IOWriter {

    explicit _IOWriter(BUS *bus, Registers *registers)
        : _RWIO(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? memory_selector(instruction)
                           : register_selector(instruction);
    }
  };
};

#endif // _MOV_H_
