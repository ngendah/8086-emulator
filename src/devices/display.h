//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "device.h"
#include "port.h"

struct Display final : Device {
  void bootstrap(Ports *ports, UNUSED_PARAM InterruptHandler *) override {
    (void)ports;
  };
};

#endif // _DISPLAY_H_
