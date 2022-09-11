//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _JE_H_
#define _JE_H_

#include "types.h"

struct JE : MicroOp {
  JE(bus_ptr_t bus, registers_ptr_t registers) : MicroOp(bus, registers) {}
};

#endif // _JE_H_