//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include "address.h"
#include "flags.h"
#include "io.h"
#include "port.h"

#include <cstdint>
#include <string>

struct Register : IO {
  explicit Register(uint16_t val = 0);

  Register(const std::string &name);

  Register(const Register &rhs);

  ~Register() override = default;

  void write_hi(const uint8_t val) override;

  void write_lo(const uint8_t val) override;

  void write(const uint16_t val) override;

  void write(const uint8_t val) override;

  uint16_t read() const override;

  uint8_t read_hi() const override;

  uint8_t read_lo() const override;

  uint8_t read_byte() const override;

  operator uint16_t() const;

  Register &operator+=(uint16_t offset);

  Register &operator-=(uint16_t offset);

  virtual Register &operator=(uint16_t val);

  std::string name() const;

  friend std::ostream &operator<<(std::ostream &ostream, const Register &rhs);

protected:
  u16_t _register;
  std::string _name;
};

struct Segment final : Register {
  explicit Segment(uint16_t val = 0);

  Segment(const Segment &rhs);

  Segment(const std::string &name);

  ~Segment() override = default;

  Segment &operator=(uint16_t val) override;

  Address address(const Register &reg, uint16_t offset = 0x0);

  Address address(uint16_t eff_addr);
};

struct Registers final {
  Register AX, BX, CX, DX, SP, BP, SI, DI, IP;
  Segment CS, DS, SS, ES;
  Flags FLAGS;
  Ports PORTS; // TODO move or rename cls

  Registers();
};

#endif // _REGISTERS_H_
