//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _OUT_H_
#define _OUT_H_

#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct OUT : public MicroOp {
  OUT(bus_ptr_t bus, registers_ptr_t registers)
      : _registers(registers), _bus(bus) {}

protected:
  registers_ptr_t _registers;
  bus_ptr_t _bus;

  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(UNUSED_PARAM const Instruction &) const {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct _IOReader final : public IOReader {
    _IOReader(bus_ptr_t bus, registers_ptr_t registers)
        : _registers(registers), _bus(bus) {}

    IO *reader(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }

    registers_ptr_t _registers;
    bus_ptr_t _bus;
  };
};

struct OUTPort : public OUT {
  OUTPort(bus_ptr_t bus, registers_ptr_t registers) : OUT(bus, registers) {}

  MICRO_OP_INSTRUCTION_OVR(OUTPort, WordOrByteMovOpTypeSelector, MovOperator)
protected:
  struct _IOWriter final : public IOWriter {
    BUSIO _io;
    _IOWriter(bus_ptr_t bus, UNUSED_PARAM registers_ptr_t) : _io(bus) {}

    IO *writer(const Instruction &instruction) override {
      _io.set_address(Address((uint8_t)instruction.port()));
      return &_io;
    }
  };
};

struct OUTDX : public OUT {
  OUTDX(bus_ptr_t bus, registers_ptr_t registers) : OUT(bus, registers) {}

  MICRO_OP_INSTRUCTION_OVR(OUTDX, WordOrByteMovOpTypeSelector, MovOperator)
protected:
  struct _RegisterSelector2 final : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::DX_INDEX;
    }
  };

  struct _IOWriter final : public IOWriter {
    registers_ptr_t _registers;

    _IOWriter(UNUSED_PARAM bus_ptr_t, registers_ptr_t registers)
        : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector2();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }
  };
};

#endif // _OUT_H_