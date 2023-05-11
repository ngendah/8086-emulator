//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef RAM_H_
#define RAM_H_

#include "address.h"
#include "bus.h"
#include "bytes.h"
#include "logger.h"
#include "types.h"

#include <memory>
#include <sstream>

struct stream_buffer : std::basic_streambuf<uint8_t> {
  stream_buffer() = default;

  stream_buffer(uint8_t *src, std::streamsize len) { setbuf(src, len); }

  std::basic_streambuf<uint8_t> *setbuf(char_type *src,
                                        std::streamsize len) override {
    auto _start = src;
    auto _end = src + len;
    setg(_start, _start, _end);
    setp(_start, _end);
    return this;
  }

  std::streamsize size() { return in_avail(); }

  /**
   * @brief size of read sequence
   *
   */
  std::streamsize psize() const { return epptr() - pbase(); }

  /**
   * @brief size of write sequence
   *
   */
  std::streamsize gsize() const { return egptr() - eback(); }

  /**
   * @brief current position of read sequence
   *
   */
  std::streamsize ppos() const { return pptr() - pbase(); }

  /**
   * @brief current position of write sequence
   *
   */
  std::streamsize gpos() const { return gptr() - eback(); }

  /**
   * @brief seek from beginning of the read sequence
   *
   */
  pos_type pseekpos(pos_type pos) {
    if (!pbounded(pos)) {
      return pos_type(off_type(-1));
    }
    pbump(pos - ppos());
    return pos;
  }

  /**
   * @brief seek from beginning of the write sequence
   *
   */
  pos_type gseekpos(pos_type pos) {
    if (!gbounded(pos)) {
      return pos_type(off_type(-1));
    }
    gbump(pos - gpos());
    return pos;
  }

  /**
   * @brief read from the read sequence
   *
   */
  std::streampos sgetn(char_type *dest, std::streamsize n) {
    return std::basic_streambuf<uint8_t>::sgetn(dest, n);
  }

  std::streamsize sgetn(char_type **dest, std::streamsize n) {
    if (reinterpret_cast<std::streamsize>(gpos()) + n >
        reinterpret_cast<std::streamsize>(egptr())) {
      return pos_type(off_type(-1));
    }
    *dest = gptr();
    gbump(n);
    return n;
  }

  pos_type seekpos(pos_type pos,
                   std::ios_base::openmode mode = std::ios_base::in |
                                                  std::ios_base::out) override {
    if ((mode & std::ios_base::in) == std::ios_base::in) {
      pbump(offset(pos, mode));
    }
    if ((mode & std::ios_base::out) == std::ios_base::out) {
      gbump(offset(pos, mode));
    }
    return pos;
  }

  off_type offset(pos_type pos,
                  std::ios_base::openmode mode = std::ios_base::in |
                                                 std::ios_base::out) const {
    if ((mode & std::ios_base::in) == std::ios_base::in) {
      return pos - ppos();
    }
    return pos - gpos();
  }

  bool pbounded(pos_type pos) const {
    return pos + reinterpret_cast<std::streamsize>(pbase()) <
           reinterpret_cast<std::streamsize>(epptr());
  }

  bool gbounded(pos_type pos) const {
    return pos + reinterpret_cast<std::streamsize>(eback()) <
           reinterpret_cast<std::streamsize>(egptr());
  }
};

struct RAM final : BUS {
  RAM(uint32_t size = 1024) : _buffer(size){};

  RAM(uint8_t *buffer, uint16_t size) : _buffer(buffer, size) {}

  ~RAM() override {}

  uint16_t write(Address const *address, const Bytes &bytes) override {
    if (_buffer.pseekpos((uint32_t)(*address)) == -1) {
      assert(false);
      return -1;
    }
    return _buffer.sputn(bytes._bytes, bytes._size);
  }

  Bytes read(Address const *address, uint16_t len) override {
    uint8_t *_p = nullptr;
    if (_buffer.gseekpos((uint32_t)*address) == -1) {
      assert(false);
      return Bytes();
    }
    auto _res = _buffer.sgetn(&_p, len);
    if (_res == -1) {
      assert(false); // TODO fix
    }
    return Bytes(_p, len);
  }

  std::streambuf *rdbuf() override {
    return reinterpret_cast<std::streambuf *>(&_buffer);
  }

protected:
  struct Buffer final : stream_buffer {
    std::shared_ptr<uint8_t> _memory{};

    Buffer(uint16_t size = 1024)
        : _memory(std::allocator<uint8_t>().allocate(size)) {
      this->setbuf(_memory.get(), size);
    }

    // TODO make it visible only to tests
    Buffer(uint8_t *buffer, uint16_t size) { this->setbuf(buffer, size); }
  };

  Buffer _buffer;
};

#endif /* RAM_H_ */
