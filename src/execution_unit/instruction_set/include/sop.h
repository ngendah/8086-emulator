//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef SOP_H
#define SOP_H

#include "types.h"

struct SOP{
  static bool is_sop(const uint8_t &code) {
    return code == SOP_ES || code == SOP_CS || code == SOP_SS || code == SOP_DS;
  }

  static const uint8_t SOP_ES = 0x26;
  static const uint8_t SOP_CS = 0x2E;
  static const uint8_t SOP_SS = 0x36;
  static const uint8_t SOP_DS = 0x3E;
  static const uint8_t NONE = 0xff; // no segment override, default value
};

#endif // SOP_CHECKER_H
