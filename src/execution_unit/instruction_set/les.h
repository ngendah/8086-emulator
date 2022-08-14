//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _LES_H_
#define _LES_H_

#include "decoders.h"
#include "io_selectors.h"
#include "math_operators.h"
#include "mov_operators.h"
#include "types.h"

struct _INCR : MicroOp {
  _INCR(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCRE(_INCR, WordMovOpTypeSelector, MathOperator,
                            IncrOpType, DRR_Decoder)

  // Direct Register-Register move decoder
  struct DRR_Decoder : Decoder {
    struct _RegisterSelector1 final : RegisterSelector {
      uint8_t REG(UNUSED_PARAM const Instruction &) const override {
        return RegisterMapper::BX_INDEX;
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

struct LES : MicroOp {
  LES(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(LES, WordMovOpTypeSelector, MovOperator, REA_Decoder)

  struct DMR_Decoder : Decoder {
    struct _RegisterSelector1 final : RegisterSelector {
      uint8_t REG(UNUSED_PARAM const Instruction &) const override {
        return RegisterMapper::BX_INDEX;
      }
    };

    struct _RegisterSelector2 final : RegisterSelector {
      uint8_t REG(UNUSED_PARAM const Instruction &) const override {
        return RegisterMapper::ES_INDEX;
      }
    };

    DMR_Decoder(bus_ptr_t bus, registers_ptr_t registers)
        : Decoder(bus, registers), _io(bus, registers) {}

    IO *source(const Instruction &instruction) override {
      UNUSED(instruction);
      return nullptr;
    }

    IO *destination(const Instruction &instruction) override {
      UNUSED(instruction);
      return nullptr;
    }

  protected:
    DirectMemoryIOSelector _io;
  };

protected:
  struct _LES : MicroOp {
    _LES(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

    MICRO_OP_INSTRUCTION_DCR(_LES, WordMovOpTypeSelector, MovOperator,
                             REA_Decoder)
  };
};

#endif // _LES_H_