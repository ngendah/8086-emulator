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

struct XCHG : public MicroOp {
  XCHG(BUS *bus, Registers *registers) : _registers(registers), _bus(bus) {}

protected:
  Registers *_registers;
  BUS *_bus;
};

struct XCHGAccumulator : public XCHG {
  XCHGAccumulator(BUS *bus, Registers *registers) : XCHG(bus, registers) {}

  MICRO_OP_INSTRUCTION_OVR(XCHGAccumulator, WordOpTypeSelector, XCHGOperator)

protected:
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(__attribute__((unused)) const Instruction &) const {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct _IOReader : IOReader {
    Registers *_registers;

    _IOReader(BUS *bus, Registers *registers) : _registers(registers) {
      UNUSED(bus);
    }

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

    _IOWriter(BUS *bus, Registers *registers) : _registers(registers) {
      UNUSED(bus);
    }

    IO *writer(const Instruction &instruction) {
      auto reg_selector = _RegisterSelector2();
      return RegisterIOSelector(_registers, &reg_selector).get(instruction);
    }
  };
};

struct XCHGRegisterMemory : public XCHG {
  XCHGRegisterMemory(BUS *bus, Registers *registers) : XCHG(bus, registers) {}

  MICRO_OP_INSTRUCTION_OVR(XCHGRegisterMemory, WordOrByteOpcodeOpTypeSelector,
                           XCHGOperator)

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

    _RWIO(BUS *bus, Registers *registers)
        : _registers(registers), _io_selector(bus, registers) {}

    IO *memory_selector(const Instruction &instruction) {
      return _io_selector.get(instruction);
    }
  };

  struct _IOReader final : _RWIO, IOReader {
    _IOReader(BUS *bus, Registers *registers) : _RWIO(bus, registers) {}

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
    _IOWriter(BUS *bus, Registers *registers) : _RWIO(bus, registers) {}

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