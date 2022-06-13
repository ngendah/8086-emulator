#ifndef _MOV_OPERATORS_H_
#define _MOV_OPERATORS_H_

#include "instruction_templates.h"
#include "types.h"

enum OpTypes {
  word,
  byte,
  high_byte,
  low_byte,
};

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
  virtual OpTypes op_type(const Instruction &instruction) = 0;
};

class MovOperator {
protected:
  IO *_source, *_destination;
  OpTypeSelector *_selector;

public:
  MovOperator(IO *source, IO *destination, OpTypeSelector *selector)
      : _source(source), _destination(destination), _selector(selector) {}

  virtual void mov(const Instruction &instruction) {
    auto _op_type = _selector->op_type(instruction);
    if (_op_type == word) {
      WordOpType(_source, _destination).execute();
    } else {
      ByteOpType(_op_type, _source, _destination).execute();
    }
  }
};

struct RegisterMovOpTypeSelector : OpTypeSelector {
  OpTypes op_type(const Instruction &instruction) override {
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
  OpTypes op_type(const Instruction &instruction) override {
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
  OpTypes op_type(const Instruction &instruction) override {
    return instruction.opcode_to<opcode_w_t>().W == 1 ? word : byte;
  }
};

struct WordMovOpTypeSelector : OpTypeSelector {
  OpTypes op_type(__attribute__((unused)) const Instruction &) override {
    return word;
  }
};

#endif // _MOV_OPERATORS_H_