//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INSTRUCTION_SET_H_
#define _INSTRUCTION_SET_H_

#include "include/sop.h"
#include "instruction_templates.h"
#include "logger.h"
#include "physical_addresser.h"
#include "sop.h"
#include "types.h"
#include <array>
#include <istream>
#include <list>

struct InstructionCode {
  std::string _memonic;
  std::array<std::string, 2> _arguments;

  InstructionCode() = default;

  InstructionCode(const InstructionCode &rhs)
      : _memonic(rhs._memonic), _arguments(rhs._arguments) {}

  InstructionCode(std::string memonic) : _memonic(memonic) {}

  InstructionCode(std::string memonic, std::string larg)
      : _memonic(memonic), _arguments({larg}) {}

  InstructionCode(std::string memonic, std::string larg, std::string rarg)
      : _memonic(memonic), _arguments({larg, rarg}) {}

  bool has_mode() const {
    if(_arguments.empty())
      return false;
    if(_arguments.size() == 1 && _arguments[0] == "E")
      return true;
    return _arguments[0] == "E" || _arguments[1] == "E";
  }

  std::pair<bool, uint8_t> has_disp(uint8_t const &mode) const {
    if (!has_mode()) {
      assert(false);
    }
    auto _mod_reg_rm = *(_mod_reg_rm_t *)&mode;
    if (_mod_reg_rm.MOD == AddressingModes::MOD::MEM_NO_DISPLACEMENT &&
        _mod_reg_rm.RM == AddressingModes::RM::DIRECT_ADDRESSING) {
      return {true, sizeof(uint16_t)};
    }
    bool _is_disp_8 = false;
    if ((_is_disp_8 =
             (_mod_reg_rm.MOD == AddressingModes::MOD::MEM_DISPLACEMENT_8)) ||
        _mod_reg_rm.MOD == AddressingModes::MOD::MEM_DISPLACEMENT_16) {
      return {true, _is_disp_8 ? sizeof(uint8_t) : sizeof(uint16_t)};
    }
    return {false, 0};
  }

  std::pair<bool, uint8_t> has_data() const {
    if(_arguments.empty())
      return {false, 0};
    if (_arguments.size() == 1 &&
        (_arguments[0].find("Iw") != std::string::npos ||
         _arguments[0].find("Ib") != std::string::npos)) {
      uint8_t size =
          _arguments[0][1] == 'b' ? sizeof(uint8_t) : sizeof(uint16_t);
      return {true, size};
    }
    bool _arg_1 = false, _arg_2 = false;
    if ((_arg_1 = _arguments[0].find("Iw") != std::string::npos ||
                  _arguments[0].find("Ib") != std::string::npos) ||
        (_arg_2 = _arguments[1].find("Iw") != std::string::npos ||
                  _arguments[1].find("Ib") != std::string::npos)) {
      if(_arg_1) {
        uint8_t size =
            _arguments[0][1] == 'b' ? sizeof(uint8_t) : sizeof(uint16_t);
        return {true, size};
      }
      if(_arg_2) {
        uint8_t size =
            _arguments[1][1] == 'b' ? sizeof(uint8_t) : sizeof(uint16_t);
        return {true, size};
      }
    }
    return {false, 0};
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

  InstructionSet();

protected:
  std::list<std::pair<MicroOp::Key, create_func_t>> instructions;
  std::map<uint8_t, InstructionCode> _instruction_code_map;
};

struct InstructionsExecutor {
  InstructionsExecutor(std::streambuf *buf, bus_ptr_t bus,
                       registers_ptr_t registers)
      : _buf(buf), _params(bus, registers) {}

  void fetch_decode_execute() {
    beg();
    while (!eof()) {
      seek(_params.registers->IP);
      auto args = fetch();
      decode(args.first)->execute(args.second);
    }
  }

  std::pair<uint8_t, Instruction> fetch() {
    uint8_t _sop = SOP::NONE, _opcode = 0;
    uint16_t _offset = 0, _data = 0; // offset == displacement
    auto _data_len = 0;
    _sop = getb();
    if (SOP::is_sop(_sop)) {
      _opcode = getb();
    } else {
      _opcode = _sop;
      _sop = SOP::NONE;
    }
    auto _code = _instruction_set.find(_opcode);
    if (_code->_arguments.empty()) {
      PLOGD << fmt::format("memonic={}, sop={:x},opcode={:x}", _code->_memonic,
                           _sop, _opcode);
      return {_opcode, Instruction(_sop, _opcode)};
    }
    uint8_t _mode = 0;
    if (_code->has_mode()) {
      _mode = getb();
      auto _has_offset = _code->has_disp(_mode);
      if (_has_offset.first) {
        auto _offset_len = _has_offset.second;
        _offset = _offset_len == sizeof(uint8_t) ? getb() : getw();
      }
    }
    uint16_t _opcode_mode = make_word(_opcode, _mode);
    auto _has_data = _code->has_data();
    if (_has_data.first) {
      _data_len = _has_data.second;
      _data = _data_len == sizeof(uint8_t) ? getb() : getw();
    }
    return {_opcode,
            _data_len == sizeof(uint8_t)
                ? Instruction(_sop, _opcode_mode, _offset, (uint8_t)_data)
                : Instruction(_sop, _opcode_mode, _offset, (uint16_t)_data)};
  }

  std::shared_ptr<MicroOp> decode(uint8_t opcode) {
    return _instruction_set.decode(opcode, _params);
  }

  uint16_t beg() {
    _params.registers->IP = 0;
    auto res = _buf->pubseekpos((uint16_t)_params.registers->IP);
    return res;
  }

  bool eof() const { return _buf->pubseekoff(0, std::ios_base::cur) == -1; }

  uint16_t seek(uint16_t pos) { return _buf->pubseekpos(pos); }

  uint8_t getb() { return _get<uint8_t>(); }

  uint16_t getw() { return _get<uint16_t>(); }

protected:
  template <typename T> T _get() {
    T _val = 0;
    _buf->sgetn((char *)&_val, sizeof(_val));
    _params.registers->IP += sizeof(_val);
    return _val;
  }

protected:
  InstructionSet _instruction_set;
  std::streambuf *_buf;
  MicroOp::Params _params;
};

#endif // _INSTRUCTION_SET_H_
