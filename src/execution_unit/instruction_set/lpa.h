//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _LPA_H_
#define _LPA_H_

#include "decoders.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct LPA : MicroOp {
  LPA(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(LPA, WordMovOpTypeSelector, MovOperator, RPA_Decoder)

  struct RPA_Decoder final : Decoder {
    struct _RegisterSelector final : RegisterSelector {
      uint8_t REG(const Instruction &instruction) const override {
        return instruction.mode_to<mod_reg_rm_t>().REG;
      }
    };

    RPA_Decoder(bus_ptr_t bus, registers_ptr_t registers)
        : Decoder(bus, registers) {}

    IO *source(const Instruction &instruction) override {
      auto io_addresser = IOAddresser(_registers, &default_memory_selector);
      auto address = io_addresser.address<PhysicalAddresser>(instruction);
      _io.write((uint16_t)address);
      return &_io;
    }

    IO *destination(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }

  protected:
    ValueIO _io;
  };
};

#endif // _LPA_H_
