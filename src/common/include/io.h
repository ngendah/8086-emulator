//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _IO_H_
#define _IO_H_

#include <cstdint>

struct IO {
  IO() = default;

  virtual ~IO() = default;

  virtual void write_hi(uint8_t val) = 0;

  virtual void write_lo(uint8_t val) = 0;

  virtual void write(uint8_t val) = 0;

  virtual void write(uint16_t val) = 0;

  virtual uint16_t read() const = 0;

  virtual uint8_t read_byte() const = 0;

  virtual uint8_t read_hi() const = 0;

  virtual uint8_t read_lo() const = 0;
};

#endif // _IO_H_
