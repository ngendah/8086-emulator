//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INSTRUCTION_SET_H_
#define _INSTRUCTION_SET_H_

#include "address.h"
#include "instruction_templates.h"
#include "logger.h"
#include "micro_op.h"
#include "physical_addresser.h"
#include "sop.h"
#include "types.h"

#include <array>
#include <list>
#include <map>
#include <memory>

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

  bool has_mod() const {
    if (_arguments.empty())
      return false;
    if (_arguments.size() == 1 && _arguments[0] == "E")
      return true;
    return _arguments[0] == "E" || _arguments[1] == "E";
  }

  std::pair<bool, uint8_t> has_disp(uint8_t const &mod) const {
    if (!has_mod()) {
      assert(false);
    }
    auto _mod_reg_rm = *(_mod_reg_rm_t *)&mod;
    if (_mod_reg_rm.MOD == AddressingModes::MOD::MEM_NO_DISPLACEMENT &&
        _mod_reg_rm.RM == AddressingModes::RM::DIRECT_ADDRESSING) {
      return {true, sizeof_ui16};
    }
    bool _is_disp_8 = false;
    if ((_is_disp_8 =
             (_mod_reg_rm.MOD == AddressingModes::MOD::MEM_DISPLACEMENT_8)) ||
        _mod_reg_rm.MOD == AddressingModes::MOD::MEM_DISPLACEMENT_16) {
      return {true, _is_disp_8 ? sizeof_ui8 : sizeof_ui16};
    }
    return {false, 0};
  }

  std::pair<bool, uint8_t> has_data() const {
    if (_arguments.empty())
      return {false, 0};
    if (_arguments.size() == 1 &&
        (_arguments[0].find("Iw") != std::string::npos ||
         _arguments[0].find("Ib") != std::string::npos)) {
      uint8_t size = _arguments[0][1] == 'b' ? sizeof_ui8 : sizeof_ui16;
      return {true, size};
    }
    bool _arg_1 = false, _arg_2 = false;
    if ((_arg_1 = _arguments[0].find("Iw") != std::string::npos ||
                  _arguments[0].find("Ib") != std::string::npos) ||
        (_arg_2 = _arguments[1].find("Iw") != std::string::npos ||
                  _arguments[1].find("Ib") != std::string::npos)) {
      if (_arg_1) {
        uint8_t size = _arguments[0][1] == 'b' ? sizeof_ui8 : sizeof_ui16;
        return {true, size};
      }
      if (_arg_2) {
        uint8_t size = _arguments[1][1] == 'b' ? sizeof_ui8 : sizeof_ui16;
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

struct InstructionUnit {
  typedef std::shared_ptr<MicroOp> (*create_func_t)(const MicroOp::Params &);

  template <typename TDerived>
  void register_instruction(const MicroOp::Key &key) {
    instructions.push_back(
        std::make_pair(key, (create_func_t)&TDerived::create));
  }

  InstructionUnit::create_func_t find(const MicroOp::Key &);
  InstructionCode *find(uint8_t opcode);

  std::shared_ptr<MicroOp> decode(uint8_t opcode,
                                  const MicroOp::Params &params);

  InstructionUnit();

protected:
  std::list<std::pair<MicroOp::Key, create_func_t>> instructions;
  std::map<uint8_t, InstructionCode> _instruction_code_map;
};

#endif // _INSTRUCTION_SET_H_
