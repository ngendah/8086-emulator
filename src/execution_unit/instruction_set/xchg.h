//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _XCHG_H_
#define _XCHG_H_

#include "instruction_templates.h"
#include "io_selectors.h"
#include "types.h"
#include "xchg_operators.h"

class XCHG : public MicroOp {
public:
  XCHG(BUS *bus, Registers *registers) : _registers(registers), _bus(bus) {}

protected:
  Registers *_registers;
  BUS *_bus;
};

class XCHGAccumulator : public XCHG {
public:
  XCHGAccumulator(BUS *bus, Registers *registers) : XCHG(bus, registers) {}

  void execute(const Instruction &instruction) override {
    auto op_type_selector = WordOpTypeSelector();
    auto io_reader = _IOReader(_registers);
    auto io_writer = _IOWriter(_registers);
    auto xchg_operator =
        XCHGOperator(io_reader.reader(instruction),
                     io_writer.writer(instruction), &op_type_selector);
    return xchg_operator.execute(instruction);
  }

  MICRO_OP_INSTRUCTION(XCHGAccumulator)

protected:
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(__attribute__((unused)) const Instruction &) const {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct _IOReader : IOReader {
    Registers *_registers;
    _IOReader(Registers *registers) : _registers(registers) {}
    IO *reader(const Instruction &instruction) {
      auto reg_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &reg_selector).get(instruction);
    }
  };

  struct _RegisterSelector2 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.opcode_to<opcode_reg_t>().REG;
    }
  };

  struct _IOWriter : IOWriter {
    Registers *_registers;
    _IOWriter(Registers *registers) : _registers(registers) {}
    IO *writer(const Instruction &instruction) {
      auto reg_selector = _RegisterSelector2();
      return RegisterIOSelector(_registers, &reg_selector).get(instruction);
    }
  };
};

class XCHGRegisterMemory : public XCHG {
public:
  XCHGRegisterMemory(BUS *bus, Registers *registers) : XCHG(bus, registers) {}

  void execute(const Instruction &instruction) override {
    auto op_type_selector = WordOrByteOpcodeOpTypeSelector();
    auto io_reader = _IOReader(_bus, _registers);
    auto io_writer = _IOWriter(_bus, _registers);
    auto xchg_operator =
        XCHGOperator(io_reader.reader(instruction),
                     io_writer.writer(instruction), &op_type_selector);
    return xchg_operator.execute(instruction);
  }

  MICRO_OP_INSTRUCTION(XCHGRegisterMemory)

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

  struct _RWIO {
    Registers *_registers;
    MemoryIOSelector _io_selector;

    explicit _RWIO(BUS *bus, Registers *registers)
        : _registers(registers), _io_selector(bus, registers) {}

    IO *memory_selector(const Instruction &instruction) {
      return _io_selector.get(instruction);
    }
  };

  struct _IOReader final : _RWIO, IOReader {
    explicit _IOReader(BUS *bus, Registers *registers)
        : _RWIO(bus, registers) {}

    IO *register_selector(const Instruction &instruction) {
      auto register_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }

    IO *reader(const Instruction &instruction) override {
      auto mode = instruction.mode_to<mod_reg_rm_t>();
      return mode.MOD == 0x3 ? register_selector(instruction)
                             : memory_selector(instruction);
    }
  };

  struct _IOWriter final : _RWIO, IOWriter {
    explicit _IOWriter(BUS *bus, Registers *registers)
        : _RWIO(bus, registers) {}

    IO *register_selector(const Instruction &instruction) {
      auto register_selector = _RegisterSelector2();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }

    IO *writer(const Instruction &instruction) override {
      auto mode = instruction.mode_to<mod_reg_rm_t>();
      return mode.MOD == 0x3 ? register_selector(instruction)
                             : memory_selector(instruction);
    }
  };
};

#endif // _XCHG_H_