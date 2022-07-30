//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef RAM_H_
#define RAM_H_

#include <algorithm>
#include <cstdint>
#include <vector>

#include "types.h"

class Buffer {
  std::vector<uint8_t> _memory;

public:
  Buffer(uint32_t size = 128) : _memory(size & 0x000FFFFF) {}

  uint16_t write(Address *address, Bytes bytes) {
    auto begin = _memory.data();
    auto end = begin + _memory.size();
    if (begin + ((uint32_t)*address) + bytes._size >= end) {
      assert(false);
    }
    auto at = begin + (uint32_t)*address;
    std::memcpy((void *)at, bytes._bytes, bytes._size);
    return bytes._size;
  }

  uint16_t write(Address *address, Extensions::Bytes bytes) {
    auto begin = _memory.data();
    auto end = begin + _memory.size();
    if (begin + ((uint32_t)*address) + bytes.size() >= end) {
      assert(false);
    }
    auto at = begin + (uint32_t)*address;
    std::memcpy((void *)at, bytes.ptr(), bytes.size());
    return bytes.size();
  }

  Bytes read(Address *address, uint16_t len) {
    auto begin = _memory.data();
    auto end = begin + _memory.size();
    if (begin + ((uint32_t)*address) >= end) {
      assert(false);
    }
    auto at = begin + (uint32_t)*address;
    return Bytes(at, len);
  }
};

class RAM : public BUS {
  Buffer _buffer;

public:
  RAM(uint32_t size = 1024 * 1024) : _buffer(size){};

  ~RAM() override {}

  uint16_t write(Address *address, const Bytes &bytes) override {
    return _buffer.write(address, bytes);
  }

  uint16_t write(Address *address, const Extensions::Bytes &bytes) override {
    return _buffer.write(address, bytes);
  }

  Bytes read(Address *address, uint16_t len) override {
    return _buffer.read(address, len);
  }
};

#endif /* RAM_H_ */
