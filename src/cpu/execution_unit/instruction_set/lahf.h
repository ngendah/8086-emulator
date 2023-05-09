//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _LAHF_H_
#define _LAHF_H_

#include "decoders.h"
#include "io_selectors.h"
#include "micro_op.h"
#include "mov_operators.h"
#include "register_mapper.h"
#include "types.h"

struct LAHF : MicroOp {
  LAHF(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(LAHF, HighLowMovOpTypeSelector, MovOperator,
                           LAHF_Decoder)

  struct HighLowMovOpTypeSelector : OpTypeSelector {
    OpTypes op_type(UNUSED_PARAM const Instruction &) const override {
      return low_high_byte;
    }
  };

  struct LAHF_Decoder final : Decoder {
    struct _RegisterSelector final : RegisterSelector {
      uint8_t REG(UNUSED_PARAM const Instruction &) const override {
        return RegisterMapper::AX_INDEX;
      }
    };

    LAHF_Decoder(bus_ptr_t bus, registers_ptr_t registers)
        : Decoder(bus, registers) {}

    IO *source(UNUSED_PARAM const Instruction &) override {
      return &_registers->FLAGS;
    }

    IO *destination(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }
  };
};

#endif // _LAHF_H_