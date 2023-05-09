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

  Bytes() : _bytes(nullptr), _size(0) {}

  explicit Bytes(uint8_t *bytes, uint16_t size) : _bytes(bytes), _size(size) {}

  Bytes(const Bytes &bytes) : _bytes(bytes._bytes), _size(bytes._size) {}

  Bytes(const Bytes &&other)  noexcept : _bytes(other._bytes), _size(other._size) {}

  ~Bytes() {
    _bytes = nullptr;
  }

  Bytes& operator=(const Bytes &&other) {
    _bytes = other._bytes;
    _size = other._size;
    return *this;
  }

  operator uint16_t() const {
    assert(_size == 2);
    return (uint16_t)((_bytes[1] << 8) | _bytes[0]); // NOLINT
  }

  operator uint8_t() const {
    assert(_size == 1);
    return _bytes[0]; // NOLINT
  }
};

#endif // _BYTES_H_
