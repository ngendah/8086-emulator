//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _MOV_OPERATORS_H_
#define _MOV_OPERATORS_H_

#include "instruction_templates.h"
#include "operators.h"

struct MovOpType : OpType {
  void execute(const OpType::Params &params) const override {
    switch (params._op_type) {
    case byte:
      params._destination->write(params._source->read_byte());
      break;
    case high_byte:
      params._destination->write_hi(params._source->read_hi());
      break;
    case low_byte:
      params._destination->write_lo(params._source->read_lo());
      break;
    case word:
      params._destination->write(params._source->read());
      break;
    case high_low_byte:
      params._destination->write_lo(params._source->read_hi());
      break;
    case low_high_byte:
      params._destination->write_hi(params._source->read_lo());
      break;
    default: // NOP
      break;
    }
  }
};

static const auto mov_op_type_operator = MovOpType();

struct MovOperator : public Operator {
  virtual ~MovOperator() = default;
  MovOperator(IO *source, IO *destination, OpTypeSelector *selector)
      : Operator(source, destination, selector, &mov_op_type_operator) {}
};

struct RegisterMovOpTypeSelector : OpTypeSelector {
  OpTypes op_type(const Instruction &instruction) const override {
    InstructionTemplate<opcode_w_t, opcode_reg_t> instruction_template(
        instruction);
    uint8_t REG = instruction_template.mode().REG;
    uint8_t W = instruction_template.opcode().W;
    auto WREG = (uint8_t)(W << 3 | REG);
    if (W == 1) {
      return word;
    } else {
      OpTypes op_type = byte;
      if (WREG <= 4) {
        op_type = low_byte;
      } else if (WREG > 4 && WREG <= 8) {
        op_type = high_byte;
      }
      return op_type;
    }
  }
};

struct ImmediateMovOpTypeSelector : OpTypeSelector {
  OpTypes op_type(const Instruction &instruction) const override {
    opcode_w_reg_t opcode = instruction.opcode_to<opcode_w_reg_t>();
    uint8_t REG = opcode.REG;
    uint8_t W = opcode.W;
    auto WREG = (uint8_t)(W << 3 | REG);
    if (W == 1) {
      return word;
    } else {
      OpTypes op_type = byte;
      if (WREG <= 4) {
        op_type = low_byte;
      } else if (WREG > 4 && WREG <= 8) {
        op_type = high_byte;
      }
      return op_type;
    }
  }
};

struct WordOrByteMovOpTypeSelector : OpTypeSelector {
  OpTypes op_type(const Instruction &instruction) const override {
    return instruction.opcode_to<opcode_w_t>().W == 1 ? word : byte;
  }
};

struct WordMovOpTypeSelector : OpTypeSelector {
  OpTypes op_type(UNUSED_PARAM const Instruction &) const override {
    return word;
  }
};

#endif // _MOV_OPERATORS_H_