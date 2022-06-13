#ifndef _MOV_OPERATORS_H_
#define _MOV_OPERATORS_H_

#include "types.h"

enum OpTypes {
  word,
  byte,
  high_byte,
  low_byte,
};

template <typename T, typename P> struct OpTypeSelector {
  virtual OpTypes
  op_type(const InstructionTemplate<T, P> &instruction_template) = 0;
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

class MovOperator {
protected:
  IO *_source, *_destination;

public:
  MovOperator(IO *source, IO *destination)
      : _source(source), _destination(destination) {}

  virtual void mov(const Instruction &instruction) = 0;
};

template <typename T, typename P>
struct WREGMovOperator : public MovOperator, public OpTypeSelector<T, P> {
  explicit WREGMovOperator(IO *source, IO *destination)
      : MovOperator(source, destination) {}

  void mov(const Instruction &instruction) override {
    InstructionTemplate<T, P> instruction_template(instruction);
    auto _op_type = op_type(instruction_template);
    if (_op_type == word) {
      WordOpType(_source, _destination).execute();
    } else {
      ByteOpType(_op_type, _source, _destination).execute();
    }
  }

  OpTypes
  op_type(const InstructionTemplate<T, P> &instruction_template) override {
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

template <typename T, typename P>
struct ExtendedWREGMovOperator : public MovOperator,
                                 public OpTypeSelector<T, P> {
  explicit ExtendedWREGMovOperator(IO *source, IO *destination)
      : MovOperator(source, destination) {}

  void mov(const Instruction &instruction) override {
    InstructionTemplate<T, P> instruction_template(instruction);
    auto _op_type = op_type(instruction_template);
    if (_op_type == word) {
      WordOpType(_source, _destination).execute();
    } else {
      ByteOpType(_op_type, _source, _destination).execute();
    }
  }

  OpTypes
  op_type(const InstructionTemplate<T, P> &instruction_template) override {
    uint8_t REG = instruction_template.opcode().REG;
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

template <typename T, typename P>
struct WMovOperator : public MovOperator, public OpTypeSelector<T, P> {
  explicit WMovOperator(IO *source, IO *destination)
      : MovOperator(source, destination) {}

  void mov(const Instruction &instruction) override {
    InstructionTemplate<T, P> instruction_template(instruction);
    auto _op_type = op_type(instruction_template);
    if (_op_type == word) {
      WordOpType(_source, _destination).execute();
    } else {
      ByteOpType(byte, _source, _destination).execute();
    }
  }

  OpTypes
  op_type(const InstructionTemplate<T, P> &instruction_template) override {
    return instruction_template.opcode().W == 1 ? word : byte;
  }
};

struct WordMovOperator : public MovOperator {
  explicit WordMovOperator(IO *source, IO *destination)
      : MovOperator(source, destination) {}

  void mov(__attribute__((unused)) const Instruction&) override {
    WordOpType(_source, _destination).execute();
  }
};

#endif // _MOV_OPERATORS_H_