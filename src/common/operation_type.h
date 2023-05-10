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
    Flags *_flags;

    Params(OpTypes op_type, IO *source, IO *destination)
        : _op_type(op_type), _jmp_type(CJmpOpTypes::noj), _source(source),
          _destination(destination), _flags(nullptr) {}

    Params(OpTypes op_type, IO *source, IO *destination, Flags *flags)
        : _op_type(op_type), _jmp_type(CJmpOpTypes::noj), _source(source),
          _destination(destination), _flags(flags) {}

    Params(OpTypes op_type, IO *source, IO *destination, Flags *flags,
           CJmpOpTypes jmp_type)
        : _op_type(op_type), _jmp_type(jmp_type), _source(source),
          _destination(destination), _flags(flags) {}

    Params &operator=(const Params &rhs) {
      _op_type = rhs._op_type;
      _jmp_type = rhs._jmp_type;
      _source = rhs._source;
      _destination = rhs._destination;
      _flags = rhs._flags;
      return *this;
    }
  };

  virtual void execute(const OpType::Params &params) const = 0;
};

#endif // _OPERATION_TYPE_H_
