//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _OPERATORS_H_
#define _OPERATORS_H_

#include "op_type.h"
#include "op_type_selector.h"
#include "types.h"

#include <string>

struct WordOrByteOpcodeOpTypeSelector : OpTypeSelector {
  OpTypes op_type(const Instruction &instruction) const override {
    return instruction.opcode_to<opcode_w_t>().W == 1 ? word : byte;
  }
};

struct WordOpTypeSelector : OpTypeSelector {
  OpTypes op_type(UNUSED_PARAM const Instruction &) const override {
    return word;
  }
};

class Operator {
protected:
  IO *_source, *_destination;
  OpTypeSelector *_selector;
  OpType *_op_type_operator;
  registers_ptr_t _registers;

public:
  virtual ~Operator() = default;
  Operator(IO *source, IO *destination, OpTypeSelector *selector,
           OpType *op_type_operator, registers_ptr_t registers = nullptr)
      : _source(source), _destination(destination), _selector(selector),
        _op_type_operator(op_type_operator), _registers(registers) {}

  virtual void execute(const Instruction &instruction) {
    auto _op_type = _selector->op_type(instruction);
    // NOLINTNEXTLINE
    PLOGD << fmt::format("mov operation type={}", _OpTypes[_op_type]);
    _op_type_operator->execute(
        OpType::Params(_op_type, _source, _destination, _registers));
  }
};

#endif // _OPERATORS_H_