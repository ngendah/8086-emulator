//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _OPERATION_TYPE_H_
#define _OPERATION_TYPE_H_

#include "types.h"

struct OpType {
  virtual ~OpType() = default;
  struct Params {
    OpTypes _op_type;
    CJmpOpTypes _jmp_type;
    IO *_source, *_destination;
    Registers *_registers;

    explicit Params(OpTypes op_type, IO *source, IO *destination)
        : _op_type(op_type), _jmp_type(CJmpOpTypes::noj), _source(source),
          _destination(destination), _registers(nullptr) {}

    Params(OpTypes op_type, IO *source, IO *destination, Registers *registers)
        : _op_type(op_type), _jmp_type(CJmpOpTypes::noj), _source(source),
          _destination(destination), _registers(registers) {}

    Params(OpTypes op_type, IO *source, IO *destination, Registers *registers,
           CJmpOpTypes jmp_type)
        : _op_type(op_type), _jmp_type(jmp_type), _source(source),
          _destination(destination), _registers(registers) {}

    Params(const Params &other)
        : _op_type(other._op_type), _jmp_type(other._jmp_type),
          _source(other._source), _destination(other._destination),
          _registers(other._registers) {}

    Params &operator=(const Params &rhs) {
      _op_type = rhs._op_type;
      _jmp_type = rhs._jmp_type;
      _source = rhs._source;
      _destination = rhs._destination;
      _registers = rhs._registers;
      return *this;
    }
  };

  virtual void execute(const OpType::Params &params) const = 0;
};

#endif // _OPERATION_TYPE_H_
