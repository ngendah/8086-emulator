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
  ~CJmpOperator() override = default;
  CJmpOperator(IO *source, IO *destination, OpTypeSelector *selector,
               OpType *const optype, registers_ptr_t registers)
      : Operator(source, destination, selector, optype, registers) {}

  void execute(const Instruction &instruction) override {
    auto _op_type = _selector->op_type(instruction);
    auto _jmp_op_type =
        dynamic_cast<CJmpOpTypeSelector *>(_selector)->jmp_type(instruction);
    // NOLINTNEXTLINE
    PLOGD << fmt::format("source_ptr=0x{0:x}, destination_ptr=0x{1:x}",
                         (long)_source, (long)_destination); // NOLINT
    // NOLINTNEXTLINE
    PLOGD << fmt::format("operation type={}", _OpTypes[_op_type]);
    // NOLINTNEXTLINE
    PLOGD << fmt::format("jump type={}", _CJmpOpTypes[(int)_jmp_op_type]);
    _op_type_operator->execute(OpType::Params(_op_type, _source, _destination,
                                              _registers, _jmp_op_type));
  }
};

struct CJmpOpType : OpType {
  void execute(const OpType::Params &params) const override {
    Register *IP = dynamic_cast<Register *>(params._source);
    IO *value = params._destination;
    switch (params._jmp_type) {
    case CJmpOpTypes::je:
      if (params._registers->FLAGS.bits<flags_t>().Z == 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jne:
      if (params._registers->FLAGS.bits<flags_t>().Z != 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::js:
      if (params._registers->FLAGS.bits<flags_t>().S == 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jns:
      if (params._registers->FLAGS.bits<flags_t>().S != 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jo:
      if (params._registers->FLAGS.bits<flags_t>().O == 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jno:
      if (params._registers->FLAGS.bits<flags_t>().O != 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jc:
    case CJmpOpTypes::jb:
    case CJmpOpTypes::jnae:
      if (params._registers->FLAGS.bits<flags_t>().C == 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jnc:
    case CJmpOpTypes::jae:
    case CJmpOpTypes::jnb:
      if (params._registers->FLAGS.bits<flags_t>().C != 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jp:
    case CJmpOpTypes::jpe:
      if (params._registers->FLAGS.bits<flags_t>().P == 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jnp:
    case CJmpOpTypes::jpo:
      if (params._registers->FLAGS.bits<flags_t>().P != 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jbe:
    case CJmpOpTypes::jna:
      if (params._registers->FLAGS.bits<flags_t>().C == 1 &&
          params._registers->FLAGS.bits<flags_t>().Z == 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jnbe:
    case CJmpOpTypes::ja:
      if (params._registers->FLAGS.bits<flags_t>().C != 1 &&
          params._registers->FLAGS.bits<flags_t>().Z != 1) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jle:
    case CJmpOpTypes::jng:
      if (params._registers->FLAGS.bits<flags_t>().S ==
              params._registers->FLAGS.bits<flags_t>().O &&
          params._registers->FLAGS.bits<flags_t>().Z == 0) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jl:
    case CJmpOpTypes::jnge:
      if (params._registers->FLAGS.bits<flags_t>().S !=
          params._registers->FLAGS.bits<flags_t>().O) {
        (*IP) += value->read();
      }
      break;
    case CJmpOpTypes::jnl:
    case CJmpOpTypes::jge:
      if (params._registers->FLAGS.bits<flags_t>().S ==
          params._registers->FLAGS.bits<flags_t>().O) {
        (*IP) += value->read();
      }
    case CJmpOpTypes::noj:
      break;
    default:
      assert(false);
    }
  }
};

#endif // _JMP_OPERATORS_H_
