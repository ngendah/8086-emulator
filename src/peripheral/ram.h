/*
 * RAM.h
 *
 */

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
    auto it = _memory.begin() + (uint32_t)*address;
    if (it == _memory.end() || it + bytes._size == _memory.end()) {
      assert(false);
    }
    std::memcpy((void *)&(*it), bytes._bytes, bytes._size);
    return bytes._size;
  }

  Bytes read(Address *address, uint16_t len) {
    auto it = _memory.begin() + (uint32_t)*address;
    if (it == _memory.end() || it + len == _memory.end()) {
      assert(false);
    }
    return Bytes(&(*it), len);
  }
};

class RAM : public BUS {
  Buffer _buffer;

public:
  RAM(uint32_t size = 1024 * 1024) : _buffer(size){};

  virtual ~RAM();

  uint16_t write(Address *address, const Bytes &bytes) {
    return _buffer.write(address, bytes);
  }

  Bytes read(Address *address, uint16_t len) {
    return _buffer.read(address, len);
  }
};

#endif /* RAM_H_ */
