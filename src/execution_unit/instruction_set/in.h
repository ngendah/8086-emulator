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
    virtual uint8_t REG(UNUSED_PARAM const Instruction &) const {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct _IOWriter : IOWriter {
    Registers *_registers;

    _IOWriter(UNUSED_PARAM bus_ptr_t, Registers *registers)
        : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }
  };
};

struct INPort : IN {
  INPort(BUS *bus, Registers *registers) : IN(registers, bus) {}

  MICRO_OP_INSTRUCTION_OVR(INPort, WordOrByteMovOpTypeSelector, MovOperator)

protected:
  struct _IOReader : IOReader {
    BUSIO _io;

    _IOReader(BUS *bus, UNUSED_PARAM registers_ptr_t) : _io(bus) {}

    IO *reader(const Instruction &instruction) override {
      _io.set_address(Address((uint8_t)instruction.port()));
      return &_io;
    }
  };
};

struct INDX : IN {
  INDX(BUS *bus, Registers *registers) : IN(registers, bus) {}

  MICRO_OP_INSTRUCTION_OVR(INDX, WordOrByteMovOpTypeSelector, MovOperator)

protected:
  struct _RegisterSelector2 : RegisterSelector {
    virtual uint8_t REG(UNUSED_PARAM const Instruction &) const {
      return RegisterMapper::DX_INDEX;
    }
  };

  struct _IOReader : IOReader {
    Registers *_registers;

    _IOReader(UNUSED_PARAM BUS *bus, Registers *registers)
        : _registers(registers) {}

    IO *reader(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector2();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }
  };
};

#endif // PORT_ADDRESSING