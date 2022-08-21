//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INCR_H_
#define _INCR_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "math_operators.h"
#include "mov_operators.h"
#include "types.h"

struct INCRRegister : MicroOp {
  INCRRegister(bus_ptr_t bus, registers_ptr_t registers)
      : MicroOp(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCRE(INCRRegister, WordMovOpTypeSelector, MathOperator,
                            IncrOpType, DRR_Decoder)

  // Register-Register move decoder
  struct DRR_Decoder : Decoder {
    struct _RegisterSelector1 final : RegisterSelector {
      uint8_t REG(UNUSED_PARAM const Instruction &instruction) const override {
        return instruction.opcode_to<opcode_reg_t>().REG;
      }
    };

    DRR_Decoder(bus_ptr_t bus, registers_ptr_t registers)
        : Decoder(bus, registers) {}

    IO *source(const Instruction &instruction) override {
      auto selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &selector).get(instruction);
    }

    IO *destination(const Instruction &instruction) override {
      auto selector = _RegisterSelector1();
      return RegisterIOSelector(_registers, &selector).get(instruction);
    }
  };
};

#endif // _INCR_H_