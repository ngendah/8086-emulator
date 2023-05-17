//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _POINTER_H_
#define _POINTER_H_

#include "device.h"
#include "port.h"

struct Pointer final : Device {
  InterruptHandler *_interrupt_handler{};

  void bootstrap(Ports *ports, InterruptHandler *handler) override {
    (void)ports;
    _interrupt_handler = handler;
  };
};

#endif // _POINTER_H_
