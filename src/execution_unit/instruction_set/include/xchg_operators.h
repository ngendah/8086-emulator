//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _XCHG_OPERATORS_H_
#define _XCHG_OPERATORS_H_

#include "instruction_templates.h"
#include "operators.h"

struct XCHGOpType : OpType {
  void execute(const OpType::Params &params) const override {
    switch (params._op_type) {
    case byte:
      auto val = (uint8_t)params._source->read_byte();
      params._destination->write(params._source->read_byte());
      params._souce->write(val);
      break;
    case high_byte:
      auto val = (uint8_t)params._source->read_hi();
      params._destination->write_hi(params._source->read_hi());
      params._source->write_hi(val);
      break;
    case low_byte:
      auto val = (uint8_t)params._source->read_lo();
      params._destination->write_lo(params._source->read_lo());
      params._source->write_lo(val);
      break;
    default:
      auto val = (uint16_t)params._destination->read();
      params._destination->write(params._source->read());
      params._source->write(val);
      break;
    }
  }
};

static const auto xchg_op_type_operator = XCHGOpType();

struct XCHGOperator : public Operator {
  XCHGOperator(IO *source, IO *destination, OpTypeSelector *selector)
      : Operator(source, destination, selector, &xchg_op_type_operator) {}
};

#endif // _XCHG_OPERATORS_H_