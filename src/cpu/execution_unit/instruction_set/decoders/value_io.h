//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _VALUE_IO_H_
#define _VALUE_IO_H_

#include "types.h"

struct ValueIO final : IO {
  explicit ValueIO() = default;

  ValueIO(const ValueIO &rhs);

  ValueIO &operator=(const uint16_t &val);

  ValueIO &operator=(const uint8_t &val);

  void write_hi(const uint8_t val) override;

  void write_lo(const uint8_t val) override;

  void write(const uint8_t val) override;

  void write(const uint16_t val) override;

  uint16_t read() const override;

  uint8_t read_byte() const override;

  uint8_t read_hi() const override;

  uint8_t read_lo() const override;

protected:
  u16_t _value;
};

#endif // _VALUE_IO_H_
