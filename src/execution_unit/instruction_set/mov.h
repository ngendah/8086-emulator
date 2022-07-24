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

struct Mov : public MicroOp {
  explicit Mov(BUS *bus, Registers *registers)
      : _bus(bus), _registers(registers) {}

protected:
  BUS *_bus;
  Registers *_registers;
};

struct MovRegisterRegister : public Mov {
  MovRegisterRegister(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  Instruction before_execute(const Instruction &instruction) override {
    auto mod = instruction.mode_to<mod_reg_rm_t>();
    PLOGD << instruction;
    PLOGD << mod;
    assert(mod.MOD == 0x3);
    return Mov::before_execute(instruction);
  }

  MICRO_OP_INSTRUCTION_OVR(MovRegisterRegister, RegisterMovOpTypeSelector,
                           MovOperator)

protected:
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

    explicit _IOReader(BUS *bus, Registers *registers) : _registers(registers) {
      UNUSED(bus);
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

    explicit _IOWriter(BUS *bus, Registers *registers) : _registers(registers) {
      UNUSED(bus);
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
};

struct MovRegisterMemory : public Mov {
  MovRegisterMemory(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  Instruction before_execute(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    assert(opcode.W == 1);
    return Mov::before_execute(instruction);
  }

  MICRO_OP_INSTRUCTION_OVR(MovRegisterMemory, WordMovOpTypeSelector,
                           MovOperator)

protected:
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct _RWIO {
    Registers *_registers;
    MemoryIOSelector _io_selector;

    _RWIO(BUS *bus, Registers *registers)
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
    _IOReader(BUS *bus, Registers *registers) : _RWIO(bus, registers) {}

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      assert(opcode.D == 0);
      return opcode.D == 1 ? memory_selector(instruction)
                           : register_selector(instruction);
    }
  };

  struct _IOWriter final : _RWIO, IOWriter {

    _IOWriter(BUS *bus, Registers *registers) : _RWIO(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? register_selector(instruction)
                           : memory_selector(instruction);
    }
  };
};

struct MovRegisterAndMemory : public Mov {
  MovRegisterAndMemory(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  void execute(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    auto params = MicroOp::Params(_bus, _registers);
    if (mode.MOD == 0x3) {
      return MovRegisterRegister::create(params)->execute(instruction);
    }
    return MovRegisterMemory::create(params)->execute(instruction);
  }

  MICRO_OP_INSTRUCTION(MovRegisterAndMemory)
};

struct MovRegisterImmediate : public Mov {
  MovRegisterImmediate(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  MICRO_OP_INSTRUCTION_OVR(MovRegisterImmediate, ImmediateMovOpTypeSelector,
                           MovOperator)
protected:
  struct _RegisterSelector : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.opcode_to<opcode_w_reg_t>().REG;
    }
  };

  struct _IOReader : IOReader {
    ValueIO _value_io;
    explicit _IOReader(__attribute__((unused)) BUS *,
                       __attribute__((unused)) Registers *) {}

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

    explicit _IOWriter(__attribute__((unused)) BUS *, Registers *registers)
        : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      return RegisterIOSelector(_registers, this).get(instruction);
    }
  };
};

struct MovRegisterSegment : public Mov {
  MovRegisterSegment(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  Instruction before_execute(const Instruction &instruction) override {
    PLOGD << instruction;
    auto ins_temp = InstructionTemplate<d_w_t, mod_sr_rm_t>(instruction);
    assert(ins_temp.opcode().W == 0);
    assert(ins_temp.mode().MOD == 0x3);
    return Mov::before_execute(instruction);
  }

  MICRO_OP_INSTRUCTION_OVR(MovRegisterSegment, WordMovOpTypeSelector,
                           MovOperator)

protected:
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
    explicit _IOReader(__attribute__((unused)) BUS *, Registers *registers)
        : _RWIO(registers) {}

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? register_selector(instruction)
                           : segment_selector(instruction);
    }
  };

  struct _IOWriter final : _RWIO, IOWriter {
    explicit _IOWriter(__attribute__((unused)) BUS *, Registers *registers)
        : _RWIO(registers) {}

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? segment_selector(instruction)
                           : register_selector(instruction);
    }
  };
};

struct MovMemorySegment : public Mov {
  MovMemorySegment(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  Instruction before_execute(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    PLOGD << opcode;
    assert(opcode.W == 0);
    return Mov::before_execute(instruction);
  }

  MICRO_OP_INSTRUCTION_OVR(MovMemorySegment, WordMovOpTypeSelector, MovOperator)

protected:
  struct _RWIO {
    Registers *_registers;
    MemoryIOSelector _io_selector;

    _RWIO(BUS *bus, Registers *registers)
        : _registers(registers), _io_selector(bus, registers) {}

    IO *segment_selector(const Instruction &instruction) {
      return SegmentIOSelector(_registers).get(instruction);
    }

    IO *memory_selector(const Instruction &instruction) {
      return _io_selector.get(instruction);
    }
  };

  struct _IOReader final : _RWIO, IOReader {
    _IOReader(BUS *bus, Registers *registers) : _RWIO(bus, registers) {}

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? memory_selector(instruction)
                           : segment_selector(instruction);
    }
  };

  struct _IOWriter final : _RWIO, IOWriter {
    _IOWriter(BUS *bus, Registers *registers) : _RWIO(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? segment_selector(instruction)
                           : memory_selector(instruction);
    }
  };
};

struct MovSegmentAndRegisterMemory : public Mov {
  explicit MovSegmentAndRegisterMemory(BUS *bus, Registers *registers)
      : Mov(bus, registers) {}

  void execute(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    auto params = MicroOp::Params(_bus, _registers);
    if (mode.MOD == 0x3) {
      return MovRegisterSegment::create(params)->execute(instruction);
    }
    return MovMemorySegment::create(params)->execute(instruction);
  }

  MICRO_OP_INSTRUCTION(MovSegmentAndRegisterMemory)
};

struct MovAccumulator : public Mov {
  MovAccumulator(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  MICRO_OP_INSTRUCTION_OVR(MovAccumulator, WordOrByteMovOpTypeSelector,
                           MovOperator)

protected:
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(__attribute__((unused)) const Instruction &) const {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct _RWIO {
    Registers *_registers;
    DirectMemoryIOSelector _io_selector;

    _RWIO(BUS *bus, Registers *registers)
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
    _IOReader(BUS *bus, Registers *registers) : _RWIO(bus, registers) {}

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? register_selector(instruction)
                           : memory_selector(instruction);
    }
  };

  struct _IOWriter final : _RWIO, IOWriter {

    _IOWriter(BUS *bus, Registers *registers) : _RWIO(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return opcode.D == 1 ? memory_selector(instruction)
                           : register_selector(instruction);
    }
  };
};

struct MovMemoryImmediate : public Mov {
  explicit MovMemoryImmediate(BUS *bus, Registers *registers)
      : Mov(bus, registers) {}

  MICRO_OP_INSTRUCTION_OVR(MovMemoryImmediate, WordOrByteMovOpTypeSelector,
                           MovOperator)

protected:
  struct _IOReader : IOReader {
    ValueIO _value_io;
    explicit _IOReader(__attribute__((unused)) BUS *,
                       __attribute__((unused)) Registers *) {}

    IO *reader(const Instruction &instruction) override {
      auto opcode_w = instruction.opcode_to<opcode_w_t>();
      if (opcode_w.W == 1)
        _value_io = instruction.data<uint16_t>();
      else
        _value_io = instruction.data<uint8_t>();
      return &_value_io;
    }
  };

  struct _IOWriter final : IOWriter {
    MemoryIOSelector _io_selector;

    _IOWriter(BUS *bus, Registers *registers) : _io_selector(bus, registers) {}

    IO *writer(const Instruction &instruction) override {
      auto mode = instruction.mode_to<mod_reg_rm_t>();
      assert(mode.REG == 0);
      return _io_selector.get(instruction);
    }
  };
};

#endif // _MOV_H_
