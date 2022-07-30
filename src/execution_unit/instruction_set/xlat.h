//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _XLAT_H_
#define _XLAT_H_

#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct XLAT : MicroOp {
  XLAT(bus_ptr_t bus, registers_ptr_t registers)
      : _bus(bus), _registers(registers) {}

  Instruction before_execute(UNUSED_PARAM const Instruction &) override {
    return Instruction(SegmentMapper::SOP_DS_INDEX, 0x0);
  }

  MICRO_OP_INSTRUCTION_OVR(XLAT, WordOrByteMovOpTypeSelector, MovOperator)
protected:
  bus_ptr_t _bus;
  registers_ptr_t _registers;

  struct _RegisterSelector final : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct _IOWriter final : IOWriter {
    registers_ptr_t _registers;

    _IOWriter(UNUSED_PARAM bus_ptr_t, registers_ptr_t registers)
        : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }
  };

  struct _MemorySelector final : MemorySelector {
    uint8_t MOD(UNUSED_PARAM const Instruction &) const override { return 0x2; }
    uint8_t RM(UNUSED_PARAM const Instruction &) const override {
      return PhysicalAddresser::AX_BX_INDEX;
    }
    SegmentMappingTypes segment_mapping_type() const override {
      return defaults;
    }
  };

  struct _IOReader final : IOReader {
    MemoryIOSelector _io;
    _MemorySelector _selector;

    _IOReader(bus_ptr_t bus, registers_ptr_t registers)
        : _io(bus, registers, &_selector) {}

    IO *reader(const Instruction &instruction) override {
      return _io.get(instruction);
    }
  };
};

#endif // _XLAT_H_