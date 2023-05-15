//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "device.h"
#include "pointer.h"

struct KeyBoard final : Device {
  struct _Port : Port {
    void write_hi(uint8_t val) override { Port::write_hi(val); }
    void write_lo(uint8_t val) override { Port::write_lo(val); }
    void write(uint8_t val) override { Port::write(val); }
    void write(uint16_t val) override { Port::write(val); }
    uint16_t read() const override { return Port::read(); }
    uint8_t read_byte() const override { return Port::read_byte(); }
    uint8_t read_hi() const override { return Port::read_hi(); }
    uint8_t read_lo() const override { return Port::read_lo(); }
    friend struct KeyBoard;

  private:
    _Port(KeyBoard *keyboard) : _keyboard(keyboard) {}

    uint8_t _control_register{};
    KeyBoard *_keyboard;
  };

  uint16_t _port_number = 0x61;
  _Port _port;
  InterruptHandler *_interrupt_handler{};

  KeyBoard() : _port(this) {}

  void process_input(uint8_t *key_state) {
    (void)key_state; // process the input, write ...
    _interrupt_handler->interrupt(0x21);
  }

protected:
  void bootstrap(Ports *ports, InterruptHandler *handler) override {
    ports->add(_port_number, &_port);
    _interrupt_handler = handler;
  }
};

#endif // _KEYBOARD_H_
