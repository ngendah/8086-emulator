#include "address.h"

#include <fmt/core.h>

Address::Address() : _address(0) {}
Address::Address(const Address &rhs) : _address(rhs._address) {}
Address::Address(const uint8_t address) : _address(address) {}
Address::Address(const uint16_t address) : _address(address) {}
Address::Address(const uint32_t address) : _address(address) {}
Address &Address::operator=(const Address &rhs) {
  _address = rhs._address;
  return *this;
}
Address::operator uint8_t() const { return (uint8_t)_address & 0x000000ff; }
Address::operator uint16_t() const { return (uint16_t)_address & 0x0000ffff; }
Address::operator uint32_t() const { return (uint32_t)_address & 0x000fffff; }

Address &Address::operator+=(uint16_t offset) {
  _address += offset;
  return *this;
}

Address &Address::operator-=(uint16_t offset) {
  _address -= offset;
  return *this;
}

Address Address::operator+(uint16_t offset) const {
  return Address(_address + offset);
}

Address Address::operator+(const Address &rhs) const {
  return Address(_address + rhs._address);
}

Address Address::operator-(const uint16_t offset) const {
  return Address(_address - offset);
}

std::ostream &operator<<(std::ostream &os, const Address &address) {
  os << fmt::format("address=0x{:x}", address._address);
  return os;
}

