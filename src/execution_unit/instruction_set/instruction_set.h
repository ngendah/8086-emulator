//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INSTRUCTION_SET_H_
#define _INSTRUCTION_SET_H_

#include "types.h"

struct Comparator {
  bool operator()(const MicroOp::Key &lhs, const MicroOp::Key &rhs) const {
    return lhs < rhs;
  }
};

struct InstructionSet {
  typedef std::shared_ptr<MicroOp> (*create_func_t)(const MicroOp::Params &);

  template <typename TDerived>
  void register_instruction(const MicroOp::Key &key) {
    instructions[key] = &TDerived::create;
  }

  InstructionSet::create_func_t find(const MicroOp::Key &);

  std::shared_ptr<MicroOp> decode(uint8_t opcode,
                                  const MicroOp::Params &params);

  InstructionSet();

protected:
  std::map<MicroOp::Key, create_func_t, Comparator> instructions;
};

#endif // _INSTRUCTION_SET_H_