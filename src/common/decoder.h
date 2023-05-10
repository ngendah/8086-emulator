//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _DECODER_H_
#define _DECODER_H_

#include "types.h"

struct Decoder {
  Decoder(bus_ptr_t bus = nullptr, registers_ptr_t registers = nullptr)
      : _bus(bus), _registers(registers) {}

  virtual ~Decoder() = default;
  virtual std::pair<IO *, IO *> decode(const Instruction &instruction) {
    return {source(instruction), destination(instruction)};
  };

protected:
  virtual IO *source(const Instruction &) = 0;
  virtual IO *destination(const Instruction &) = 0;

  bus_ptr_t _bus;
  registers_ptr_t _registers;
};

#endif // _DECODER_H_
