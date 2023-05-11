#include "flags.h"

#include <cassert>

Flags::Flags() : _flags(0) {}

Flags::Flags(const Flags &rhs) : _flags(rhs._flags) {}

void Flags::set(uint16_t flags) { _flags = flags; }

Flags::operator uint16_t() const { return _flags.word; }

std::ostream &operator<<(UNUSED_PARAM std::ostream &ostream,
                         UNUSED_PARAM const Flags &) {
  // instead do: os << flags.bits<flags_t>();
  assert(false);
  // return ostream;
}

void Flags::write_hi(const uint8_t val) { _flags.hi = val; }

void Flags::write_lo(const uint8_t val) { _flags.lo = val; }

void Flags::write(const uint16_t val) { _flags.word = val; }

uint16_t Flags::read() const { return _flags.word; }

uint8_t Flags::read_hi() const { return _flags.hi; }

uint8_t Flags::read_lo() const { return _flags.lo; }

void Flags::write(UNUSED_PARAM const uint8_t) { assert(false); }

uint8_t Flags::read_byte() const {
  assert(false);
  // return (uint8_t)_flags.lo;
}