#include "value_io.h"

ValueIO::ValueIO(const ValueIO &rhs) : _value(rhs._value) {}

ValueIO &ValueIO::operator=(const uint16_t &val) {
  _value.word = val;
  return *this;
}

ValueIO &ValueIO::operator=(const uint8_t &val) {
  _value.lo = val;
  return *this;
}

void ValueIO::write_hi(const uint8_t val) { _value.hi = val; }

void ValueIO::write_lo(const uint8_t val) { _value.lo = val; }

void ValueIO::write(const uint8_t val) { _value.lo = val; }

void ValueIO::write(const uint16_t val) { _value.word = val; }

uint16_t ValueIO::read() const { return _value.word; }

uint8_t ValueIO::read_byte() const { return _value.lo; }

uint8_t ValueIO::read_hi() const { return _value.hi; }

uint8_t ValueIO::read_lo() const { return _value.lo; }
