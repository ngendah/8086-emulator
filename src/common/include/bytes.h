//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _BYTES_H_
#define _BYTES_H_

#include <cassert>
#include <cstdint>

struct Bytes {
  uint8_t *_bytes;
  uint16_t _size;

  Bytes();

  explicit Bytes(uint8_t *bytes, uint16_t size);

  Bytes(const Bytes &bytes);

  Bytes(const Bytes &&other) noexcept;

  ~Bytes();

  Bytes &operator=(const Bytes &&other);

  operator uint16_t() const;

  operator uint8_t() const;
};

#endif // _BYTES_H_
