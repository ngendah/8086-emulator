//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _MOV_OPERATORS_H_
#define _MOV_OPERATORS_H_

#include "instruction_templates.h"
#include "operators.h"

struct MovOperator : public Operator {
  MovOperator(IO *source, IO *destination, OpTypeSelector *selector)
      : Operator(source, destination, selector) {}

  // TODO refactor by moving to base class
  virtual void mov(const Instruction &instruction) {
    auto _op_type = _selector->op_type(instruction);
    PLOGD << fmt::format("source_ptr=0x{0:x}, destination_ptr=0x{1:x}",
                         (long)_source, (long)_destination);
    PLOGD << fmt::format("mov operation type={}", _OpTypes[_op_type]);
    if (_op_type == word) {
      WordOpType(_source, _destination).execute();
    } else {
      ByteOpType(_op_type, _source, _destination).execute();
    }
  }
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
  OpTypes op_type(__attribute__((unused)) const Instruction &) const override {
    return word;
  }
};

#endif // _MOV_OPERATORS_H_