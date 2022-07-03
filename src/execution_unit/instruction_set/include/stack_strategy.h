//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _STACK_STRATEGY_H_
#define _STACK_STRATEGY_H_

#include "types.h"

struct StackStrategy {
  virtual void next(Register &, uint16_t) const = 0;
  virtual void prev(Register &, uint16_t) const = 0;
};

struct StackFullDescending : StackStrategy {
  void next(Register &register_, uint16_t count) const override {
    register_ -= count;
  };

  void prev(Register &register_, uint16_t count) const override {
    register_ += count;
  };
};

static const auto stack_full_descending = StackFullDescending();

struct StackFullAscending : StackStrategy {
  void next(Register &register_, uint16_t count) const override {
    register_ += count;
  };

  void prev(Register &register_, uint16_t count) const override {
    register_ -= count;
  };
};

static const auto stack_full_ascending = StackFullAscending();

#endif // _STACK_STRATEGY_H_