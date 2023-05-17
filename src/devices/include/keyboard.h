//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "device.h"
#include "pointer.h"

struct KeyBoard final : Device {
  struct _Port final : Port {
    friend struct KeyBoard;

  private:
    _Port(KeyBoard *keyboard) : _keyboard(keyboard) {}
    KeyBoard *_keyboard;
  };

  const uint16_t _port_number = 0x61;
  _Port _port;
  InterruptHandler *_interrupt_handler{};

  KeyBoard() : _port(this) {}

  void process_input(uint8_t *key_state) {
    if (key_state == nullptr) {
      return _interrupt_handler->interrupt(0x19);
    }
    _interrupt_handler->interrupt(0x21);
  }

protected:
  void bootstrap(Ports *ports, InterruptHandler *handler) override {
    ports->add(_port_number, &_port);
    _interrupt_handler = handler;
  }
};

#endif // _KEYBOARD_H_
