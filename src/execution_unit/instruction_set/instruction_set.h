//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INSTRUCTION_SET_H_
#define _INSTRUCTION_SET_H_

#include "include/sop.h"
#include "types.h"
#include <array>
#include <istream>
#include <list>

struct InstructionCode {
  uint8_t _opcode;
  std::string _memonic;
  std::array<std::string, 2> _arguments;

  InstructionCode() : _opcode(0) {}

  InstructionCode(uint8_t opcode) : _opcode(opcode) {}

  InstructionCode(uint8_t opcode, std::string memonic)
      : _opcode(opcode), _memonic(memonic) {}

  InstructionCode(uint8_t opcode, std::string memonic, std::string larg)
      : _opcode(opcode), _memonic(memonic), _arguments({larg, std::string()}) {}

  InstructionCode(uint8_t opcode, std::string memonic, std::string larg,
                  std::string rarg)
      : _opcode(opcode), _memonic(memonic), _arguments({larg, rarg}) {}
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
  InstructionCode *find(uint8_t opcode);

  std::shared_ptr<MicroOp> decode(uint8_t opcode,
                                  const MicroOp::Params &params);

  std::shared_ptr<MicroOp> decode(InstructionCode const *code,
                                  const MicroOp::Params &params);

  InstructionSet();

protected:
  std::list<std::pair<MicroOp::Key, create_func_t>> instructions;
  std::map<uint8_t, InstructionCode> _instruction_code_map;
};

struct InstructionsExecutor {
  InstructionsExecutor() : _instruction_buff(nullptr) {}
  InstructionsExecutor(std::streambuf &instruction_buff, bus_ptr_t bus,
                       registers_ptr_t registers)
      : _instruction_buff(&instruction_buff), _params(bus, registers) {}

  void fetch_and_execute() {
    while (!_instruction_buff.eof()) {
      _instruction_buff.seekg((uint16_t)_params.registers->IP,
                              std::ios_base::beg);
      // TODO decompose into separate fetch and execute functions
      uint8_t _sop = 0, _opcode = 0;
      _sop = get<uint8_t>();
      if (SOP::is_sop(_sop)) {
        _opcode = get<uint8_t>();
      } else {
        _opcode = _sop;
        _sop = 0;
      }
      auto code = _instruction_set.find(_opcode);
      if (code->_arguments.empty()) {
        _instruction_set.decode(code, _params)
            ->execute(Instruction(_sop, _opcode));
      } else {
        uint8_t _mode = 0;
        if (has_mode(code)) {
          _mode = get<uint8_t>();
        }
        uint16_t _opcode_mode = make_word(_opcode, _mode);
        UNUSED(_opcode_mode);
        // TODO complete implementation
      }
    }
  }

protected:
  bool has_mode(InstructionCode const *code);

  template <typename T> T get() {
    T _val = 0;
    _instruction_buff.get((char *)&_val, sizeof(T));
    _params.registers->IP += sizeof(T);
    return _val;
  }

protected:
  InstructionSet _instruction_set;
  std::istream _instruction_buff;
  MicroOp::Params _params;
};

#endif // _INSTRUCTION_SET_H_
