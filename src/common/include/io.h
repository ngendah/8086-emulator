//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _IO_H_
#define _IO_H_

#include "types.h"

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

  struct u16 {
    u16(const u16 &val) : _parent(val._parent){};

    void write(uint16_t val) { _parent->write(val); }

    void write(u16_t val) { _parent->write(val.word); }

    void write_hi(uint8_t val) { _parent->write_hi(val); }

    void write_lo(uint8_t val) { _parent->write_lo(val); }

    uint16_t read() const { return _parent->read(); }

    uint8_t read_hi() const { return _parent->read_hi(); }

    uint8_t read_lo() const { return _parent->read_lo(); }

    friend struct IO;

  private:
    u16(IO *parent) : _parent(parent) {}
    IO *_parent;
  };

  u16 to_u16() { return u16(dynamic_cast<IO *>(this)); }

  struct u8 {
    u8(const u8 &val) : _parent(val._parent){};

    void write(uint8_t val) { _parent->write(val); }

    uint8_t read() const { return _parent->read(); }

    friend struct IO;

  private:
    u8(IO *parent) : _parent(parent) {}
    IO *_parent;
  };

  u8 to_u8() { return u8(dynamic_cast<IO *>(this)); }
};

#endif // _IO_H_
