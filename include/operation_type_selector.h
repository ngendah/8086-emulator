//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _OPERATION_TYPE_SELECTOR_H_
#define _OPERATION_TYPE_SELECTOR_H_

#include "types.h"

struct OpTypeSelector {
  virtual ~OpTypeSelector() = default;
  virtual OpTypes op_type(const Instruction &instruction) const = 0;
};

#endif // _OPERATION_TYPE_SELECTOR_H_
