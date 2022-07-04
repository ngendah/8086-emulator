//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _XCHG_OPERATORS_H_
#define _XCHG_OPERATORS_H_

#include "instruction_templates.h"
#include "operators.h"

class XCHGWordOpType : protected OpType {
public:
  XCHGWordOpType(IO *source, IO *destination) : OpType(source, destination) {}
  void execute() override {
    auto val = (uint16_t)_destination->read();
    _destination->write(_source->read());
    return _source->write(val);
  }
};

class XCHGByteOpType : protected OpType {
protected:
  OpTypes _op_type;

public:
  XCHGByteOpType(OpTypes op_type, IO *source, IO *destination)
      : OpType(source, destination), _op_type(op_type) {}

  void execute() override {
    switch (_op_type) {
    case high_byte:
      auto val = (uint8_t)_source->read_hi();
      _destination->write_hi(_source->read_hi());
      _source->write_hi(val);
      break;
    case low_byte:
      auto val = (uint8_t)_source->read_lo();
      _destination->write_lo(_source->read_lo());
      _source->write_lo(val);
      break;
    default:
      auto val = (uint8_t)_source->read_byte();
      _destination->write(_source->read_byte());
      _souce->write(val);
      break;
    }
  }
};

struct XCHGOperator : public Operator {
  XCHGOperator(IO *source, IO *destination, OpTypeSelector *selector)
      : Operator(source, destination, selector) {}

  // TODO refactor by moving to base class
  virtual void xchg(const Instruction &instruction) {
    auto _op_type = _selector->op_type(instruction);
    PLOGD << fmt::format("source_ptr=0x{0:x}, destination_ptr=0x{1:x}",
                         (long)_source, (long)_destination);
    PLOGD << fmt::format("mov operation type={}", _OpTypes[_op_type]);
    if (_op_type == word) {
      XCHGWordOpType(_source, _destination).execute();
    } else {
      XCHGByteOpType(_op_type, _source, _destination).execute();
    }
  }
};

#endif // _XCHG_OPERATORS_H_