//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include <cstdint>

struct Address final {
  Address();
  Address(const Address &rhs);
  Address(const uint8_t address);
  Address(const uint16_t address);
  Address(const uint32_t address);
  Address &operator=(const Address &rhs);
  operator uint8_t() const;
  operator uint16_t() const;
  operator uint32_t() const;
  Address &operator+=(uint16_t offset);
  Address operator+(uint16_t offset) const;
  Address operator+(const Address &rhs) const;

protected:
  uint32_t _address;
};

#endif // _ADDRESS_H_
