#include "registers.h"

#include <fmt/core.h>

Register::Register(uint16_t val) : _register(val) {}

Register::Register(uint16_t val, const std::string &name)
    : _register(val), _name(name) {}

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
  const uint16_t eff_addr = offset + cast_ui16(reg);
  const uint32_t segment_addr = cast_ui32(*this);
  return Address(cast_ui32(segment_addr * 0x10 + eff_addr));
}

Address Segment::address(uint16_t eff_addr) {
  const uint32_t segment_addr = cast_ui32(*this);
  return Address(cast_ui32(segment_addr * 0x10 + eff_addr));
}

StackPointer::StackPointer(uint16_t val)
    : Register(val), empty(true), _base(val) {}

StackPointer::StackPointer(const std::string &name)
    : Register(name), empty(true), _base(0) {}

StackPointer::StackPointer(const StackPointer &rhs)
    : Register(rhs), empty(rhs.empty), _base(rhs._base) {}

StackPointer::operator uint16_t() const {
  return Register::operator uint16_t();
}

Register *StackPointer::operator&() { return dynamic_cast<Register *>(this); }

StackPointer &StackPointer::operator+=(uint16_t offset) {
  Register::operator+=(offset);
  return *this;
}

StackPointer &StackPointer::operator-=(uint16_t offset) {
  Register::operator-=(offset);
  return *this;
}

StackPointer &StackPointer::operator=(uint16_t val) {
  _base = val;
  Register::operator=(val);
  return *this;
}

bool StackPointer::at_base() const { return _base == _register; }

Registers::Registers()
    : AX("AX"), BX("BX"), CX("CX"), DX("DX"), BP("BP"), SI("SI"), DI("DI"),
      IP("IP"), CS("CS"), DS("DS"), SS("SS"), ES("ES"), SP("SP") {}
