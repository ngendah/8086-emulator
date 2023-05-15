//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _PORT_H_
#define _PORT_H_

#include "io.h"

#include <map>

struct Port : IO {
  Port() = default;

  ~Port() override = default;

  void write_hi(uint8_t val) override;

  void write_lo(uint8_t val) override;

  void write(uint8_t val) override;

  void write(uint16_t val) override;

  uint16_t read() const override;

  uint8_t read_byte() const override;

  uint8_t read_hi() const override;

  uint8_t read_lo() const override;

private:
  u16_t _buffer;
};

struct Ports final {
  Ports() = default;

  ~Ports() = default;

  Port *port(uint8_t number) const;

  void add(uint8_t number, Port *port);

#ifndef _TESTS_H_
private:
#endif
  std::map<uint8_t, Port *> _ports;
};

#endif // _PORT_H_
