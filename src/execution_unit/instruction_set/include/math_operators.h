//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _MATH_OPERATORS_H_
#define _MATH_OPERATORS_H_

#include "instruction_templates.h"
#include "operators.h"

struct MathOperator : Operator {
  MathOperator(IO *source, IO *destination, OpTypeSelector *selector,
               OpType *const optype, Flags *flags)
      : Operator(source, destination, selector, optype), _flags(flags) {}

  void execute(const Instruction &instruction) override {
    auto _op_type = _selector->op_type(instruction);
    PLOGD << fmt::format("source_ptr=0x{0:x}, destination_ptr=0x{1:x}",
                         (long)_source, (long)_destination);
    PLOGD << fmt::format("math operation type={}", _OpTypes[_op_type]);
    _op_type_operator->execute(
        OpType::Params(_op_type, _source, _destination, _flags));
  }

protected:
  Flags *_flags;
};

struct IncrOpType : OpType {
  void execute(const OpType::Params &params) const override {
    auto flags = params._flags->bits<flags_t>();
    uint16_t val = params._source->read();
    auto incr_by = params._op_type == OpTypes::word ? 2 : 1;
    flags.C = (val + incr_by) == UINT16_MAX ? 1 : 0;
    val += incr_by;
    params._destination->write(val);
    params._flags->set((uint16_t)flags);
  }
};

#endif // _MATH_OPERATORS_H_
