#include "registers.h"

#include <fmt/core.h>

Register::Register(uint16_t val) : _register(val) {}

Register::Register(const std::string &name) : _register(0), _name(name) {}

Register::Register(const Register &rhs)
    : _register(rhs._register), _name(rhs._name) {}

void Register::write_hi(const uint8_t val) { _register.hi = val; }

void Register::write_lo(const uint8_t val) { _register.lo = val; }

void Register::write(const uint16_t val) { _register.word = val; }

void Register::write(const uint8_t val) { _register.lo = val; }

uint16_t Register::read() const { return _register.word; }

uint8_t Register::read_hi() const { return _register.hi; }

uint8_t Register::read_lo() const { return _register.lo; }

uint8_t Register::read_byte() const { return _register.lo; }

Register::operator uint16_t() const { return _register.word; }

Register &Register::operator+=(uint16_t offset) {
  _register.word += offset;
  return *this;
}

Register &Register::operator-=(uint16_t offset) {
  _register.word -= offset;
  return *this;
}

Register &Register::operator=(uint16_t val) {
  _register.word = val;
  return *this;
}

std::string Register::name() const { return _name; }

std::ostream &operator<<(std::ostream &ostream, const Register &rhs) {
  ostream << fmt::format("{0:}=0x{1:x}", rhs._name, rhs._register.word);
  return ostream;
}

Segment::Segment(uint16_t val) : Register(val){};

Segment::Segment(const Segment &rhs) : Register(rhs) {}

Segment::Segment(const std::string &name) : Register(name) {}

Segment &Segment::operator=(uint16_t val) {
  _register.word = val;
  return *this;
}

Address Segment::address(const Register &reg, uint16_t offset) {
  uint16_t eff_addr = offset + (uint16_t)reg;
  return Address((uint32_t)(((uint32_t) * this) * 0x10 + eff_addr));
}

Address Segment::address(uint16_t eff_addr) {
  return Address((uint32_t)(((uint32_t) * this) * 0x10 + eff_addr));
}

Registers::Registers()
    : AX("AX"), BX("BX"), CX("CX"), DX("DX"), SP("SP"), BP("BP"), SI("SI"),
      DI("DI"), IP("IP"), CS("CS"), DS("DS"), SS("SS"), ES("ES") {}