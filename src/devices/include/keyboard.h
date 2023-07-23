//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "bus.h"
#include "device.h"
#include "types.h"

struct KeyBoard final : Device {
  InterruptHandler *_interrupt_handler{};

  void process_input(uint8_t *key_state) {
    if (key_state == nullptr) {
      return _interrupt_handler->interrupt(0x19);
    }
    _interrupt_handler->interrupt(0x21);
  }

protected:
  void initialize(InterruptHandler *handler) override {
    _interrupt_handler = handler;
  }

  uint16_t write(UNUSED_PARAM Address const *,
                 UNUSED_PARAM const Bytes &) override {
    assert(0);
  }
  Bytes read(UNUSED_PARAM Address const *, UNUSED_PARAM uint16_t) override {
    assert(0);
  }
};

#endif // _KEYBOARD_H_
