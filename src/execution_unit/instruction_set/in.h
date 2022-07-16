//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _IN_H_
#define _IN_H_

#include "instruction_templates.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct IN : public MicroOp {
  IN(Registers *registers, BUS *bus) : _registers(registers), _bus(bus) {}

protected:
  Registers *_registers;
  BUS *_bus;

  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(__attribute__((unused)) const Instruction &) const {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct _IOWriter : IOWriter {
    Registers *_registers;

    _IOWriter(Registers *registers) : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }
  };
};

struct INPort : IN {
  INPort(BUS *bus, Registers *registers) : IN(registers, bus) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordOrByteMovOpTypeSelector();
    auto io_reader = _IOReader(_bus);
    auto io_writer = _IOWriter(_registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
  }

  MICRO_OP_INSTRUCTION(INPort)

protected:
  struct _IOReader : IOReader {
    BUSIO _io;

    _IOReader(BUS *bus) : _io(bus) {}

    IO *reader(const Instruction &instruction) override {
      _io.set_address(Address((uint16_t)instruction.port()));
      return &_io;
    }
  };
};

struct INDX : IN {

  INDX(BUS *bus, Registers *registers) : IN(registers, bus) {}

  void execute(const Instruction &instruction) override {
    auto op_selector = WordOrByteMovOpTypeSelector();
    auto io_reader = _IOReader(_registers);
    auto io_writer = _IOWriter(_registers);
    auto mov_operator =
        MovOperator(io_reader.reader(instruction),
                    io_writer.writer(instruction), &op_selector);
    mov_operator.execute(instruction);
  }

  MICRO_OP_INSTRUCTION(INPort)

protected:
  struct _RegisterSelector2 : RegisterSelector {
    virtual uint8_t REG(__attribute__((unused)) const Instruction &) const {
      return RegisterMapper::DX_INDEX;
    }
  };

  struct _IOReader : IOReader {
    Registers *_registers;

    _IOReader(Registers *registers) : _registers(registers) {}

    IO *reader(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector2();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }
  };
};

#endif // PORT_ADDRESSING