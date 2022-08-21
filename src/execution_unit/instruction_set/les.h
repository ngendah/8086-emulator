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

/*
LES DX, [DI]
1. load DX with EA of DI. (MOV DX, [DI])
2. Increment DX by word size i.e. DX+=2. [(INCR DX), (INCR DX)]
3. load ES with contents of DS:[DX]. (MOV ES, DX)

Test cases:
LES DX, [DI]
LES DX, 52h[DI]
LES BX, 42H[SI]

Implementation:
step 1.
MovRegisterAndMemory.execute(instruction);

step 2.
reg = instruction.mode_to<mod_reg_t>().REG;
opcode_reg_t opcode = {.OPCODE=0x5, .REG=reg};
instruction = Instruction(0xff, make_word(opcode, 0));
Incr.execute(instruction);
Incr.execute(instruction);

step 3.
-- intel-format, set d bit to 1
opcode_d_w_t opcode = {.opcode=0x0, .d=1, .w=1};
mode_reg_rm mode = {.mode=0x3, .reg=ES_INDEX, .rm=reg};
instruction = Instruction(0xff, make_word(opcode, mode));
MovRegisterRegister.execute(instruction);
 */

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