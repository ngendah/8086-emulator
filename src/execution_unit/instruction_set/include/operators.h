//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _OPERATORS_H_
#define _OPERATORS_H_

#include <string>

#include "types.h"

enum OpTypes {
  word,
  byte,
  high_byte,
  low_byte,
};

static const std::string _OpTypes[] = {"word", "byte", "high_byte", "low_byte"};

struct OpType {
  struct Params {
    OpTypes _op_type;
    IO *_source, *_destination;
    Params(OpTypes op_type, IO *source, IO *destination)
        : _op_type(op_type), _source(source), _destination(destination) {}
    Params &operator=(const Params &rhs) {
      _op_type = rhs._op_type;
      _source = rhs._source;
      _destination = rhs._destination;
      return *this;
    }
  };
  virtual void execute(const OpType::Params &params) const = 0;
};

struct OpTypeSelector {
  virtual OpTypes op_type(const Instruction &instruction) const = 0;
};

struct WordOrByteOpcodeOpTypeSelector : OpTypeSelector {
  OpTypes op_type(const Instruction &instruction) const override {
    return instruction.opcode_to<opcode_w_t>().W == 1 ? word : byte;
  }
};

struct WordOpTypeSelector : OpTypeSelector {
  OpTypes op_type(__attribute__((unused)) const Instruction &) const override {
    return word;
  }
};

class Operator {
protected:
  IO *_source, *_destination;
  OpTypeSelector *_selector;
  OpType const *_op_type_operator;

public:
  Operator(IO *source, IO *destination, OpTypeSelector *selector,
           OpType const *op_type_operator)
      : _source(source), _destination(destination), _selector(selector),
        _op_type_operator(op_type_operator) {}

  virtual void execute(const Instruction &instruction) {
    auto _op_type = _selector->op_type(instruction);
    PLOGD << fmt::format("source_ptr=0x{0:x}, destination_ptr=0x{1:x}",
                         (long)_source, (long)_destination);
    PLOGD << fmt::format("mov operation type={}", _OpTypes[_op_type]);
    _op_type_operator->execute(OpType::Params(_op_type, _source, _destination));
  }
};

#endif // _OPERATORS_H_