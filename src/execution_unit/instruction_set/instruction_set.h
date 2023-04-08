//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INSTRUCTION_SET_H_
#define _INSTRUCTION_SET_H_

#include "include/sop.h"
#include "instruction_templates.h"
#include "types.h"
#include <array>
#include <istream>
#include <list>

struct InstructionCode {
  uint8_t _opcode;
  std::string _memonic;
  std::array<std::string, 2> _arguments;

  InstructionCode() = default;

  InstructionCode(const InstructionCode &rhs)
      : _opcode(rhs._opcode), _memonic(rhs._memonic),
        _arguments(rhs._arguments) {}

  InstructionCode(uint16_t opcode) : _opcode(opcode) {}

  InstructionCode(uint16_t opcode, std::string memonic)
      : _opcode(opcode), _memonic(memonic) {}

  InstructionCode(uint16_t opcode, std::string memonic, std::string larg)
      : _opcode(opcode), _memonic(memonic), _arguments({larg}) {}

  InstructionCode(uint16_t opcode, std::string memonic, std::string larg,
                  std::string rarg)
      : _opcode(opcode), _memonic(memonic), _arguments({larg, rarg}) {}

  bool has_mode() const {
    if(_arguments.empty())
      return false;
    if(_arguments.size() == 1 && _arguments[0] == "E")
      return true;
    return _arguments[0] == "E" || _arguments[1] == "E";
  }

  std::pair<bool, uint8_t> has_disp(uint8_t const &mode) const {
    auto mod_reg_rm = *(mod_reg_rm_t*)&mode;
    if(mod_reg_rm.MOD == 0x1 || mod_reg_rm.MOD == 0x2) {
      return std::pair<bool, uint8_t>(true, mod_reg_rm.MOD==0x1?sizeof(uint8_t) : sizeof(uint16_t));
    }
    return std::pair<bool, uint8_t>(false, 0);
  }

  std::pair<bool, uint8_t> has_data() {
    if(_arguments.empty())
      return std::pair<bool, uint8_t>(false, 0);
    if (_arguments.size() == 1 &&
        (_arguments[0].find("Iw") != std::string::npos ||
         _arguments[0].find("Ib") != std::string::npos)) {
      uint8_t size =
          _arguments[0][1] == 'b' ? sizeof(uint8_t) : sizeof(uint16_t);
      return std::pair<bool, uint8_t>(true, size);
    }
    bool _arg_1 = false, _arg_2 = false;
    if ((_arg_1 = _arguments[0].find("Iw") != std::string::npos ||
                  _arguments[0].find("Ib") != std::string::npos) ||
        (_arg_2 = _arguments[1].find("Iw") != std::string::npos ||
                  _arguments[1].find("Ib") != std::string::npos)) {
      if(_arg_1) {
        uint8_t size =
            _arguments[0][1] == 'b' ? sizeof(uint8_t) : sizeof(uint16_t);
        return std::pair<bool, uint8_t>(true, size);
      }
      if(_arg_2) {
        uint8_t size =
            _arguments[1][1] == 'b' ? sizeof(uint8_t) : sizeof(uint16_t);
        return std::pair<bool, uint8_t>(true, size);
      }
    }
    return std::pair<bool, uint8_t>(false, 0);
  }
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
  InstructionsExecutor(std::streambuf &instruction_buff, bus_ptr_t bus,
                       registers_ptr_t registers)
      : _instruction_buff(&instruction_buff), _params(bus, registers) {}

  void fetch_decode_execute() {
    while (!_instruction_buff.eof()) {
      _instruction_buff.seekg((uint16_t)_params.registers->IP,
                              std::ios_base::beg);
      auto args = fetch();
      decode(args.first)->execute(args.second);
    }
  }

  std::pair<InstructionCode *, Instruction> fetch() {
    uint8_t _sop = 0, _opcode = 0;
    uint16_t _offset = 0, _data = 0; // offset == displacement
    auto _data_len = 0;
    _sop = get<uint8_t>();
    if (SOP::is_sop(_sop)) {
      _opcode = get<uint8_t>();
    } else {
      _opcode = _sop;
      _sop = 0;
    }
    auto _code = _instruction_set.find(_opcode);
    if (_code->_arguments.empty()) {
      return {_code, Instruction(_sop, _opcode)};
    }
    uint8_t _mode = 0;
    if (_code->has_mode()) {
      _mode = get<uint8_t>();
      auto _has_offset = _code->has_disp(_mode);
      if (_has_offset.first) {
        auto _offset_len = _has_offset.second;
        _offset =
            _offset_len == sizeof(uint8_t) ? get<uint8_t>() : get<uint16_t>();
      }
    }
    uint16_t _opcode_mode = make_word(_opcode, _mode);
    auto _has_data = _code->has_data();
    if (_has_data.first) {
      _data_len = _has_data.second;
      _data = _data_len == sizeof(uint8_t) ? get<uint8_t>() : get<uint16_t>();
    }
    return {_code,
            _data_len == sizeof(uint8_t)
                ? Instruction(_sop, _opcode_mode, _offset, (uint8_t)_data)
                : Instruction(_sop, _opcode_mode, _offset, (uint16_t)_data)};
  }

  std::shared_ptr<MicroOp> decode(InstructionCode const *code) {
    return _instruction_set.decode(code, _params);
  }

protected:
  template <typename T> T get() {
    T _val = 0;
    _instruction_buff.get((char *)&_val, sizeof(T));
    _params.registers->IP += sizeof(T);
    return _val;
  }

  InstructionSet _instruction_set;
  std::istream _instruction_buff;
  MicroOp::Params _params;
};

#endif // _INSTRUCTION_SET_H_
