//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _LEA_H_
#define _LEA_H_

#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct LEA : MicroOp {
  LEA(bus_ptr_t bus, registers_ptr_t registers)
      : _bus(bus), _registers(registers) {}

  MICRO_OP_INSTRUCTION_OVR(LEA, WordMovOpTypeSelector, MovOperator)
protected:
  bus_ptr_t _bus;
  registers_ptr_t _registers;

  struct _IOReader : IOReader {
    registers_ptr_t _registers;
    ValueIO _io;
    _IOReader(UNUSED_PARAM bus_ptr_t, registers_ptr_t registers)
        : _registers(registers) {}

    IO *reader(const Instruction &instruction) override {
      auto io_addresser = IOAddresser(_registers, &default_memory_selector);
      auto address = io_addresser.address<EffectiveAddresser>(instruction);
      _io.write((uint16_t)address);
      return &_io;
    }
  };

  struct _RegisterSelector final : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct _IOWriter : IOWriter {
    registers_ptr_t _registers;

    _IOWriter(UNUSED_PARAM bus_ptr_t, registers_ptr_t registers)
        : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }
  };
};

#endif // _LEA_H_