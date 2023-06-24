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
      PLOGD << fmt::format("SP={0:d}, value={1:d}, 0x{1:x}",
                           (uint16_t)params._registers->SP,
                           params._source->to_u16().read()); // NOLINT
      // the stack grow down
      // change to have endianess in the downward direction
      auto *dest = reinterpret_cast<BUSIO *>(params._destination);
      assert(dest != nullptr);
      const auto address = dest->address();
      dest->set_address(address - (uint16_t)1);
      dest->to_u16().write(params._source->to_u16().read_reversed());
      dest->set_address(address);
      params._registers->SP -= 2;
    } break;
    case byte: {
      auto *dest = reinterpret_cast<BUSIO *>(params._destination);
      assert(dest != nullptr);
      PLOGD << fmt::format("SP={0:d}, value={1:d}, 0x{1:x}",
                           (uint16_t)params._registers->SP,
                           params._source->to_u8().read()); // NOLINT
    }

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
      // reverse the order in which the word was written
      params._destination->to_u16().write(src->to_u16().read_reversed());
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
