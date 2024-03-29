//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _CONDITIONAL_JMP_H_
#define _CONDITIONAL_JMP_H_

#include "decoders.h"
#include "instruction_templates.h"
#include "jmp_operators.h"
#include "logger.h"
#include "micro_op.h"
#include "operators.h"
#include "types.h"

#define CONDITIONAL_JMP_MICRO_OP(cls_name, jmp)                                \
  struct cls_name final : MicroOp {                                            \
    cls_name(bus_ptr_t bus, registers_ptr_t registers)                         \
        : MicroOp(bus, registers) {}                                           \
                                                                               \
    struct JMP_Decoder final : Decoder {                                       \
      JMP_Decoder(bus_ptr_t bus, registers_ptr_t registers)                    \
          : Decoder(bus, registers) {}                                         \
                                                                               \
      IO *source(UNUSED_PARAM const Instruction &) override {                  \
        return &_registers->IP;                                                \
      }                                                                        \
                                                                               \
      IO *destination(const Instruction &instruction) override {               \
        _value_io = instruction.data<uint16_t>();                              \
        return &_value_io;                                                     \
      }                                                                        \
                                                                               \
    protected:                                                                 \
      ValueIO _value_io;                                                       \
    };                                                                         \
                                                                               \
    struct JmpSelector final : CJmpOpTypeSelector {                            \
      CJmpOpTypes jmp_type(UNUSED_PARAM const Instruction &) const override {  \
        return CJmpOpTypes::jmp;                                               \
      }                                                                        \
                                                                               \
      OpTypes op_type(UNUSED_PARAM const Instruction &) const override {       \
        return word;                                                           \
      }                                                                        \
    };                                                                         \
                                                                               \
    typedef ExecuteStrategy21<JmpSelector, CJmpOperator, CJmpOpType>           \
        ExecuteStrategy4;                                                      \
                                                                               \
    MICRO_OP_INSTRUCTION_D(cls_name, ExecuteStrategy4, JMP_Decoder)            \
  };

CONDITIONAL_JMP_MICRO_OP(JE, je)
CONDITIONAL_JMP_MICRO_OP(JNE, jne)
CONDITIONAL_JMP_MICRO_OP(JS, js)
CONDITIONAL_JMP_MICRO_OP(JNS, jns)
CONDITIONAL_JMP_MICRO_OP(JO, jo)
CONDITIONAL_JMP_MICRO_OP(JNO, jno)
CONDITIONAL_JMP_MICRO_OP(JC, jc)
CONDITIONAL_JMP_MICRO_OP(JNC, jnc)
CONDITIONAL_JMP_MICRO_OP(JP, jp)
CONDITIONAL_JMP_MICRO_OP(JNP, jnp)
CONDITIONAL_JMP_MICRO_OP(JBE, jbe)
CONDITIONAL_JMP_MICRO_OP(JNA, jna)
CONDITIONAL_JMP_MICRO_OP(JNBE, jnbe)
CONDITIONAL_JMP_MICRO_OP(JA, ja)
CONDITIONAL_JMP_MICRO_OP(JLE, jle)
CONDITIONAL_JMP_MICRO_OP(JNG, jng)
CONDITIONAL_JMP_MICRO_OP(JNLE, jnle)
CONDITIONAL_JMP_MICRO_OP(JG, jg)
CONDITIONAL_JMP_MICRO_OP(JL, jl)
CONDITIONAL_JMP_MICRO_OP(JNGE, jnge)
CONDITIONAL_JMP_MICRO_OP(JNL, jnl)
CONDITIONAL_JMP_MICRO_OP(JGE, jge)

#include "dec.h"
#include "register_mapper.h"

template <class T> struct LoopT : MicroOp {
  LoopT(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(const Instruction &instruction) {
    opcode_reg_t opcode = {};
    opcode.REG = RegisterMapper::CX_INDEX;
    DECRegister(_bus, _registers)
        .execute(Instruction(0xff, make_word(cast_ui8(opcode), 0)));
    T(_bus, _registers).execute(instruction);
  }

  MICRO_OP_INSTRUCTION(LoopT<T>)
};

typedef LoopT<JNE> LOOP;

struct JmpC : MicroOp {
  JmpC(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(const Instruction &instruction) {
    if (_registers->CX.read() == 0) {
      _registers->IP += instruction.data<uint16_t>();
    }
  }

  MICRO_OP_INSTRUCTION(JmpC)
};

struct JmpZ : MicroOp {
  JmpZ(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(const Instruction &instruction) {
    if (_registers->CX.read() != 0 &&
        _registers->FLAGS.bits<flags_t>().Z == 1) {
      _registers->IP += instruction.data<uint16_t>();
    }
  }

  MICRO_OP_INSTRUCTION(JmpZ)
};

struct JmpNZ : MicroOp {
  JmpNZ(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}

  void execute(const Instruction &instruction) {
    if (_registers->CX.read() != 0 &&
        _registers->FLAGS.bits<flags_t>().Z == 0) {
      _registers->IP += instruction.data<uint16_t>();
    }
  }

  MICRO_OP_INSTRUCTION(JmpNZ)
};

typedef LoopT<JmpC> JCXZ;
typedef LoopT<JmpZ> LOOPZ;
typedef LoopT<JmpNZ> LOOPNZ;

#endif // _CONDITIONAL_JMP_H_
