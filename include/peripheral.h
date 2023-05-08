//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _PERIPHERAL_H_
#define _PERIPHERAL_H_

#include <cstdint>
#include <streambuf>

struct InterruptHandler {
  virtual ~InterruptHandler() = default;
  virtual void interrupt(uint8_t type) = 0;
};

struct Peripheral {
  virtual ~Peripheral() = default;
  virtual void bootstrap(std::streambuf *, InterruptHandler *) = 0;
};

#endif // _PERIPHERAL_H_
