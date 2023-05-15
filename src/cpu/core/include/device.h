//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "port.h"

#include <cstdint>
#include <streambuf>

struct InterruptHandler {
  virtual ~InterruptHandler() = default;
  virtual void interrupt(uint8_t type) = 0;
};

struct Device { // TODO rename to Device
  virtual ~Device() = default;
  virtual void bootstrap(Ports *, InterruptHandler *) = 0;
};

#endif // _DEVICE_H_
