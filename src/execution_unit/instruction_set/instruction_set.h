//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INSTRUCTION_SET_H_
#define _INSTRUCTION_SET_H_

#include "types.h"
#include <list>
#include <array>
#include <istream>

struct InstructionCode {
  uint8_t _opcode;
  uint8_t _mode;
  std::string _memonic;
  std::array<std::string, 2> _arguments;

  InstructionCode() : _opcode(0), _mode(0) {}

  InstructionCode(uint8_t opcode) : _opcode(opcode), _mode(0) {}

  InstructionCode(uint8_t opcode, std::string memonic)
      : _opcode(opcode), _memonic(memonic) {}

  InstructionCode(uint8_t opcode, std::string memonic, std::string larg)
      : _opcode(opcode), _memonic(memonic), _arguments({larg, std::string()}) {}

  InstructionCode(uint8_t opcode, std::string memonic, std::string larg,
                  std::string rarg)
      : _opcode(opcode), _memonic(memonic), _arguments({larg, rarg}) {}
};

struct InstructionCodeReader {
  InstructionCode _opcode;
  std::istream _instruction_buff;
};

struct Comparator {
  bool operator()(const MicroOp::Key &lhs, const MicroOp::Key &rhs) const {
    return lhs < rhs;
  }
};

struct InstructionSet {
  typedef std::shared_ptr<MicroOp> (*create_func_t)(const MicroOp::Params &);

  template <typename TDerived>
  void register_instruction(const MicroOp::Key &key) {
    instructions.push_back(
        std::make_pair(key, (create_func_t)&TDerived::create));
  }

  InstructionSet::create_func_t find(const MicroOp::Key &);

  std::shared_ptr<MicroOp> decode(uint8_t opcode,
                                  const MicroOp::Params &params);

  InstructionSet();

protected:
  std::list<std::pair<MicroOp::Key, create_func_t>> instructions;
  std::map<uint8_t , InstructionCode> _instruction_code_map;
};

#endif // _INSTRUCTION_SET_H_