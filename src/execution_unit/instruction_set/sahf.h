//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _SAHF_H_
#define _SAHF_H_

#include "decoders.h"
#include "io_selectors.h"
#include "micro_op.h"
#include "mov_operators.h"
#include "register_mapper.h"
#include "types.h"

struct SAHF : MicroOp {
  SAHF(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(SAHF, HighLowMovOpTypeSelector, MovOperator,
                           SAHF_Decoder)

  struct HighLowMovOpTypeSelector : OpTypeSelector {
    OpTypes op_type(UNUSED_PARAM const Instruction &) const override {
      return high_low_byte;
    }
  };

  struct SAHF_Decoder final : Decoder {
    struct _RegisterSelector final : RegisterSelector {
      uint8_t REG(UNUSED_PARAM const Instruction &) const override {
        return RegisterMapper::AX_INDEX;
      }
    };

    SAHF_Decoder(bus_ptr_t bus, registers_ptr_t registers)
        : Decoder(bus, registers) {}

    IO *source(const Instruction &instruction) override {
      auto register_selector = _RegisterSelector();
      return RegisterIOSelector(_registers, &register_selector)
          .get(instruction);
    }

    IO *destination(UNUSED_PARAM const Instruction &) override {
      return &_registers->FLAGS;
    }
  };
};

#endif // _SAHF_H_