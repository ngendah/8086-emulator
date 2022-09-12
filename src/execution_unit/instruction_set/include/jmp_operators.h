//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _JMP_OPERATORS_H_
#define _JMP_OPERATORS_H_

#include "logger.h"
#include "operators.h"
#include "types.h"

struct CJmpOpTypeSelector : OpTypeSelector {
  virtual CJmpOpTypes jmp_type(const Instruction &instruction) const = 0;
};

struct CJmpOperator : Operator {
  CJmpOperator(IO *source, IO *destination, OpTypeSelector *selector,
               OpType *const optype, Flags *flags)
      : Operator(source, destination, selector, optype), _flags(flags) {}

  void execute(const Instruction &instruction) override {
    auto _op_type = _selector->op_type(instruction);
    auto _jmp_op_type =
        ((CJmpOpTypeSelector *)_selector)->jmp_type(instruction);
    PLOGD << fmt::format("source_ptr=0x{0:x}, destination_ptr=0x{1:x}",
                         (long)_source, (long)_destination);
    PLOGD << fmt::format("operation type={}", _OpTypes[_op_type]);
    PLOGD << fmt::format("jump type={}", _CJmpOpTypes[_jmp_op_type]);
    _op_type_operator->execute(
        OpType::Params(_op_type, _source, _destination, _flags, _jmp_op_type));
  }

protected:
  Flags *_flags;
};

struct CJmpOpType : OpType {
  void execute(const OpType::Params &params) const override {
    Register *IP = (Register *)params._source;
    IO *value = params._destination;
    switch (params._jmp_type) {
    case je:
      if (params._flags->bits<flags_t>().Z == 1) {
        (*IP) += value->read();
      }
      break;
    case jne:
      if (params._flags->bits<flags_t>().Z != 1) {
        (*IP) += value->read();
      }
      break;
    case js:
      if (params._flags->bits<flags_t>().S == 1) {
        (*IP) += value->read();
      }
      break;
    case jns:
      if (params._flags->bits<flags_t>().S != 1) {
        (*IP) += value->read();
      }
      break;
    case jo:
      if (params._flags->bits<flags_t>().O == 1) {
        (*IP) += value->read();
      }
      break;
    case jno:
      if (params._flags->bits<flags_t>().O != 1) {
        (*IP) += value->read();
      }
      break;
    case jc:
    case jb:
    case jnae:
      if (params._flags->bits<flags_t>().C == 1) {
        (*IP) += value->read();
      }
      break;
    case jnc:
    case jae:
    case jnb:
      if (params._flags->bits<flags_t>().C != 1) {
        (*IP) += value->read();
      }
      break;
    case jp:
    case jpe:
      if (params._flags->bits<flags_t>().P == 1) {
        (*IP) += value->read();
      }
      break;
    case jnp:
    case jpo:
      if (params._flags->bits<flags_t>().P != 1) {
        (*IP) += value->read();
      }
      break;
    case jbe:
    case jna:
      if (params._flags->bits<flags_t>().C == 1 &&
          params._flags->bits<flags_t>().Z == 1) {
        (*IP) += value->read();
      }
      break;
    case jnbe:
    case ja:
      if (params._flags->bits<flags_t>().C != 1 &&
          params._flags->bits<flags_t>().Z != 1) {
        (*IP) += value->read();
      }
      break;
    case jle:
    case jng:
      if (params._flags->bits<flags_t>().S ==
              params._flags->bits<flags_t>().O &&
          params._flags->bits<flags_t>().Z == 0) {
        (*IP) += value->read();
      }
      break;
    case jl:
    case jnge:
      if (params._flags->bits<flags_t>().S !=
          params._flags->bits<flags_t>().O) {
        (*IP) += value->read();
      }
      break;
    case jnl:
    case jge:
      if (params._flags->bits<flags_t>().S ==
          params._flags->bits<flags_t>().O) {
        (*IP) += value->read();
      }
    case noj:
      break;
    default:
      assert(false);
    }
  }
};

#endif // _JMP_OPERATORS_H_