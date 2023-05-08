//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef RAM_H_
#define RAM_H_

#include "types.h"
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
  pos_type ppos() const { return pptr() - pbase(); }

  /**
   * @brief current position of write sequence
   *
   */
  pos_type gpos() const { return gptr() - eback(); }

  /**
   * @brief seek from beginning of the read sequence
   *
   */
  pos_type pseekpos(pos_type pos) {
    if (pos > psize() - ppos()) {
      return pos_type(off_type(-1));
    }
    _M_in_cur = pbase() + pos;
    return pos;
  }

  /**
   * @brief seek from beginning of the write sequence
   *
   */
  pos_type gseekpos(pos_type pos) {
    if (pos > gsize() - gpos()) {
      return pos_type(off_type(-1));
    }
    _M_out_cur = eback() + pos;
    return pos;
  }

  /**
   * @brief write to the output sequence
   *
   */
  std::streamsize xsputn(const char_type *src, std::streamsize n) override {
    if (gpos() < gsize() - n) {
      return pos_type(off_type(-1));
    }
    std::memmove(static_cast<void *>(gptr()), src, n);
    gbump(n);
    return n;
  }

  /**
   * @brief read from the read sequence
   *
   */
  std::streamsize xsgetn(char_type *dest, std::streamsize n) override {
    if (ppos() < psize() - n) {
      return pos_type(off_type(-1));
    }
    std::memcpy(static_cast<void *>(dest), pptr(), n);
    pbump(n);
    return n;
  }

  std::streamsize sgetn(char_type **dest, std::streamsize n) {
    if (ppos() < psize() - n) {
      return pos_type(off_type(-1));
    }
    *dest = pptr();
    pbump(n);
    return n;
  }

  pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                   std::ios_base::openmode mode = std::ios_base::in |
                                                  std::ios_base::out) override {
    char_type **_start = {};
    char_type **_cur = {};
    uint8_t _dir = 1;
    if ((dir & std::ios_base::beg) == std::ios_base::beg) {
      _start = mode == std::ios_base::in ? &_M_out_beg : &_M_in_beg;
      _cur = mode == std::ios_base::in ? &_M_out_cur : &_M_in_cur;
      if (off > psize() - (long)_cur || off > gsize() - (long)_cur) {
        return pos_type(off_type(-1));
      }
      *_cur = _dir * off + *_start; // NOLINT
      return mode == std::ios_base::in ? ppos() : gpos();
    }
    if ((dir & std::ios_base::end) == std::ios_base::end) {
      _dir = -1;
      _start = mode == std::ios_base::in ? &_M_out_end : &_M_in_end;
      *_start = _dir * off + *_start; // NOLINT
      return mode == std::ios_base::in ? ppos() : gpos();
    }
    return pos_type(off_type(-1));
  }

  pos_type seekpos(pos_type pos,
                   std::ios_base::openmode mode = std::ios_base::in |
                                                  std::ios_base::out) override {
    char_type **_start = mode == std::ios_base::in ? &_M_out_beg : &_M_in_beg;
    char_type **_cur = mode == std::ios_base::in ? &_M_out_cur : &_M_in_cur;
    char_type **_end = mode == std::ios_base::in ? &_M_out_end : &_M_in_end;
    if ((long)(pos) < (long)(*_start) &&
        (long)(pos) > (long)(*_end)) { // NOLINT
      return pos_type(off_type(-1));
    }
    *_cur = pos + *_start;
    return pos_type(pos);
  }
};

struct RAM final : BUS {
  RAM(uint32_t size = 1024) : _buffer(size){};

  ~RAM() override {}

  uint16_t write(Address const *address, const Bytes &bytes) override {
    if (_buffer.gseekpos((uint32_t)(*address)) == -1) {
      assert(false);
      return -1;
    }
    return _buffer.sputn(bytes._bytes, bytes._size);
  }

  Bytes read(Address const *address, uint16_t len) override {
    uint8_t *_p = nullptr;
    if (_buffer.pseekpos((uint32_t)*address) == -1) {
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
    std::shared_ptr<uint8_t[]> _memory; // NOLINT

    Buffer(uint16_t size = 1024)
        : _memory(std::allocator<uint8_t>().allocate(size)) {
      this->setbuf(_memory.get(), size);
    }
  };

  Buffer _buffer;
};

#endif /* RAM_H_ */
