//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "device_io.h"

#include <cstdint>

struct InterruptHandler {
  virtual ~InterruptHandler() = default;
  virtual void interrupt(uint8_t type) = 0;
};

struct Device : DeviceIO {
  virtual ~Device() = default;
  virtual void initialize(InterruptHandler *) = 0;
};

#endif // _DEVICE_H_
