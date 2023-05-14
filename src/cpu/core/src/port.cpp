#include "port.h"

#include "logger.h"

#include <fmt/core.h>

void Port::write_hi(uint8_t val) { _buffer.hi = val; }

void Port::write_lo(uint8_t val) { _buffer.lo = val; }

void Port::write(uint8_t val) { _buffer.lo = val; }

void Port::write(uint16_t val) { _buffer.word = val; }

uint16_t Port::read() const { return _buffer.word; }

uint8_t Port::read_byte() const { return _buffer.lo; }

uint8_t Port::read_hi() const { return _buffer.hi; }

uint8_t Port::read_lo() const { return _buffer.lo; }

Port *Ports::port(uint8_t number) const {
  try {
    return _ports.at(number);
  } catch (std::out_of_range &) {
    PLOGD << fmt::format("port {} does not exist", number);
  }
  return nullptr;
}

void Ports::add(uint8_t number, Port *port) {
  auto _p = _ports.find(number);
  if (_p != _ports.end()) {
    PLOGD << fmt::format("port {} already exists", number);
    assert(false);
  }
  _ports.insert({number, port});
}
