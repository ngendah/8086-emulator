//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _POINTER_H_
#define _POINTER_H_

#include "bus.h"
#include "device.h"
#include "types.h"

struct Pointer final : Device {
  InterruptHandler *_interrupt_handler{};

  void initialize(InterruptHandler *handler) override {
    _interrupt_handler = handler;
  };

protected:
  uint16_t write(UNUSED_PARAM Address const *,
                 UNUSED_PARAM const Bytes &) override {
    assert(0);
  }
  Bytes read(UNUSED_PARAM Address const *, UNUSED_PARAM uint16_t) override {
    assert(0);
  }
};

#endif // _POINTER_H_
