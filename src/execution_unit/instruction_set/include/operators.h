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

class OpType {
protected:
  IO *_source;
  IO *_destination;

public:
  OpType(IO *source, IO *destination)
      : _source(source), _destination(destination) {}
  virtual void execute() = 0;
};

class WordOpType : protected OpType {
public:
  WordOpType(IO *source, IO *destination) : OpType(source, destination) {}
  void execute() override { return _destination->write(_source->read()); }
};

class ByteOpType : protected OpType {
protected:
  OpTypes _op_type;

public:
  ByteOpType(OpTypes op_type, IO *source, IO *destination)
      : OpType(source, destination), _op_type(op_type) {}
  void execute() override {
    switch (_op_type) {
    case high_byte:
      _destination->write_hi(_source->read_hi());
      break;
    case low_byte:
      _destination->write_lo(_source->read_lo());
      break;
    default:
      _destination->write(_source->read_byte());
      break;
    }
  }
};

struct OpTypeSelector {
  virtual OpTypes op_type(const Instruction &instruction) const = 0;
};

class Operator {
protected:
  IO *_source, *_destination;
  OpTypeSelector *_selector;

public:
  Operator(IO *source, IO *destination, OpTypeSelector *selector)
      : _source(source), _destination(destination), _selector(selector) {}
};

#endif // _OPERATORS_H_