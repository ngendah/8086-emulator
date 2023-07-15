//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _LES_H_
#define _LES_H_

#include "address.h"
#include "decoders.h"
#include "incr.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "math_operators.h"
#include "micro_op.h"
#include "mov.h"
#include "mov_operators.h"
#include "segment_mapper.h"
#include "types.h"

struct LES : MicroOp {
  LES(
      bus_ptr_t bus, registers_ptr_t registers,
      std::function<void(LES *const, uint8_t)> test_point = [](LES *const,
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

  MICRO_OP_INSTRUCTION(LES)

protected:
  void _execute(const Instruction &instruction) {
    MovRegisterMemory(_bus, _registers).execute(instruction);
    _test_point(this, 0);
    auto rm = instruction.mode_to<mod_reg_rm_t>().RM;
    {
      d_w_t opcode = {};
      opcode.D = 1;
      mod_reg_rm_t mode = {};
      mode.MOD = AddressingModes::MOD::MEM_DISPLACEMENT_16;
      mode.REG = SegmentMapper::ES_INDEX;
      mode.RM = rm;
      auto _instruction = Instruction(instruction);
      _instruction.sop(SegmentMapper::SOP_DS_INDEX);
      _instruction.opcode(opcode);
      _instruction.mode(mode);
      _instruction.offset(_instruction.offset() + sizeof_ui16);
      MovMemorySegment(_bus, _registers).execute(_instruction);
    }
  }
  std::function<void(LES *const, uint8_t)> _test_point;
};

#endif // _LES_H_
