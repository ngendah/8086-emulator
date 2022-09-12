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
        return jmp;                                                            \
      }                                                                        \
                                                                               \
      OpTypes op_type(UNUSED_PARAM const Instruction &) const override {       \
        return word;                                                           \
      }                                                                        \
    };                                                                         \
                                                                               \
    typedef _ExecuteStrategy21<JmpSelector, CJmpOperator, CJmpOpType>          \
        ExecuteStrategy21;                                                     \
                                                                               \
    MICRO_OP_INSTRUCTION_D(cls_name, ExecuteStrategy21, JMP_Decoder)           \
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

#endif // _CONDITIONAL_JMP_H_