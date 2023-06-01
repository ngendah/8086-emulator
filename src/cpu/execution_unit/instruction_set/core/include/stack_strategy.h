//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _STACK_STRATEGY_H_
#define _STACK_STRATEGY_H_

#include "op_type.h"

struct StackFullDescending {
  static void next(const OpType::Params &params) {
    switch (params._op_type) {
    case word: {
      auto *dest = reinterpret_cast<BUSIO *>(params._destination);
      assert(dest != nullptr);
      auto address = dest->address();
      dest->to_u8().write(params._source->to_u16().read_lo());
      dest->set_address(address - (uint16_t)1);
      dest->to_u8().write(params._source->to_u16().read_hi());
      dest->set_address(address);
      params._registers->SP -= 2;
    } break;
    case byte:
      params._destination->to_u8().write(params._source->to_u8().read());
      params._registers->SP -= 1;
      break;
    default:
      assert(0);
      break;
    }
  };

  static void prev(const OpType::Params &params) {
    switch (params._op_type) {
    case word: {
      auto *src = reinterpret_cast<BUSIO *>(params._source);
      assert(src != nullptr);
      auto address = src->address();
      params._destination->to_u16().write_hi(src->to_u8().read());
      src->set_address(address + (uint16_t)1);
      params._destination->to_u16().write_lo(src->to_u8().read());
      src->set_address(address);
      params._registers->SP += 2;
    } break;
    case byte:
      params._destination->to_u8().write(params._source->to_u8().read());
      params._registers->SP += 1;
      break;
    default:
      assert(0);
      break;
    }
  };
};

struct StackFullAscending {
  static void next(const OpType::Params &params) {
    switch (params._op_type) {
    case word:
      params._destination->to_u16().write(params._source->to_u16().read());
      params._registers->SP += 2;
      break;
    case byte:
      params._destination->to_u8().write(params._source->to_u8().read());
      params._registers->SP += 1;
      break;
    default:
      assert(0);
      break;
    }
  };

  static void prev(const OpType::Params &params) {
    switch (params._op_type) {
    case word:
      params._destination->to_u16().write(params._source->to_u16().read());
      params._registers->SP -= 2;
      break;
    case byte:
      params._destination->to_u8().write(params._source->to_u8().read());
      params._registers->SP -= 1;
      break;
    default:
      assert(0);
      break;
    }
  };
};

#endif // _STACK_STRATEGY_H_