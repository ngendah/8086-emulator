#include "bytes.h"

Bytes::Bytes() : _bytes(nullptr), _size(0) {}

Bytes::Bytes(uint8_t *bytes, uint16_t size) : _bytes(bytes), _size(size) {}

Bytes::Bytes(const Bytes &bytes) : _bytes(bytes._bytes), _size(bytes._size) {}

Bytes::Bytes(const Bytes &&other) noexcept
    : _bytes(other._bytes), _size(other._size) {}

Bytes::~Bytes() { _bytes = nullptr; }

Bytes &Bytes::operator=(const Bytes &&other) {
  _bytes = other._bytes;
  _size = other._size;
  return *this;
}

Bytes::operator uint16_t() const {
  assert(_size == 2);
  return (uint16_t)((_bytes[1] << 8) | _bytes[0]); // NOLINT
}

Bytes::operator uint8_t() const {
  assert(_size == 1);
  return _bytes[0]; // NOLINT
}
