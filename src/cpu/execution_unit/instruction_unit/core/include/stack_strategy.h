//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _STACK_STRATEGY_H_
#define _STACK_STRATEGY_H_

#include "op_type.h"

struct StackFullDescending {
  static Address next_addr(const OpType::Params &params, uint16_t offset) {
    auto *dest = reinterpret_cast<BUSIO *>(params._destination);
    assert(dest != nullptr);
    if (params._registers->SP.empty) {
      params._registers->SP.empty = false;
    } else {
      params._registers->SP -= offset;
      dest->set_address(dest->address() - offset);
    }
    return dest->address();
  }

  static Address prev_addr(const OpType::Params &params, uint16_t offset) {
    auto *src = reinterpret_cast<BUSIO *>(params._source);
    assert(src != nullptr);
    if (params._registers->SP.at_base()) {
      params._registers->SP.empty = true;
    } else {
      params._registers->SP += offset;
      src->set_address(src->address() + offset);
    }
    return src->address();
  }

  // condition:
  // StackMemoryIOSelector will have set the physical address of the current SP
  // refer: io_selectors.h::StackMemoryIOSelector
  static void push(const OpType::Params &params) {
    switch (params._op_type) {
    case word: {
      next_addr(params, 1);
      auto val = params._source->to_u16();
      params._destination->to_u8().write(val.read_lo());
      next_addr(params, 1);
      params._destination->to_u8().write(val.read_hi());
    } break;
    case byte:
      next_addr(params, 1);
      params._destination->to_u8().write(params._source->to_u8().read());
      break;
    default:
      assert(0);
      break;
    }
  }

  static void pop(const OpType::Params &params) {
    switch (params._op_type) {
    case word: {
      auto hi = params._source->to_u8().read();
      prev_addr(params, 1);
      auto lo = params._source->to_u8().read();
      params._destination->to_u16().write(make_word(hi, lo));
      prev_addr(params, 1);
    } break;
    case byte:
      params._destination->to_u8().write(params._source->to_u8().read());
      prev_addr(params, 1);
      break;
    default:
      assert(0);
      break;
    }
  }
};

struct StackFullAscending {
  static void push(const OpType::Params &) { assert(0); }

  static void pop(const OpType::Params &) { assert(0); }
};

#endif // _STACK_STRATEGY_H_
