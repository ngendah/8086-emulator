//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _LDS_H_
#define _LDS_H_

#include "decoders.h"
#include "incr.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "math_operators.h"
#include "mov.h"
#include "mov_operators.h"
#include "segment_mapper.h"
#include "types.h"

struct LDS : MicroOp {
  LDS(
      bus_ptr_t bus, registers_ptr_t registers,
      std::function<void(LDS *const, uint8_t)> test_point = [](LDS *const,
                                                               uint8_t) {})
      : MicroOp(bus, registers), _test_point(test_point) {}

  Instruction before_decode(const Instruction &instruction) override {
    auto _instruction = Instruction(instruction);
    auto opcode = _instruction.opcode_to<d_w_t>();
    opcode.D = 1;
    opcode.W = 1;
    _instruction.opcode(opcode);
    return _instruction;
  }

  void execute(const Instruction &instruction) override {
    auto _instruction = this->before_decode(instruction);
    this->_execute(_instruction);
  }

  MICRO_OP_INSTRUCTION(LDS)

protected:
  void _execute(const Instruction &instruction) {
    MovRegisterMemory(_bus, _registers).execute(instruction);
    _test_point(this, 0);
    auto reg = instruction.mode_to<mod_reg_rm_t>().REG;
    {
      opcode_reg_t opcode = {};
      opcode.REG = reg;
      opcode.OPCODE = 0x5;
      auto instruction = Instruction(0xff, make_word(opcode, 0));
      INCRRegister(_bus, _registers).execute(instruction);
      _test_point(this, 1);
    }
    {
      d_w_t opcode = {};
      opcode.D = 1;
      opcode.W = 1;
      mod_reg_rm_t mode;
      mode.MOD = AddressingModes::MOD::REG;
      mode.REG = RegisterMapper::SI_INDEX;
      mode.RM = reg;
      auto instruction = Instruction(0xff, make_word(opcode, mode));
      MovRegisterRegister(_bus, _registers).execute(instruction);
      _test_point(this, 2);
    }
    {
      d_w_t opcode = {};
      opcode.D = 1;
      mod_sr_rm_t mode = {};
      mode.SR = SegmentMapper::DS_INDEX;
      mode.RM = AddressingModes::RM::SI;
      auto instruction =
          Instruction(SegmentMapper::SOP_DS_INDEX, make_word(opcode, mode));
      MovMemorySegment(_bus, _registers).execute(instruction);
    }
  }
  std::function<void(LDS *const, uint8_t)> _test_point;
};

#endif // _LDS_H_