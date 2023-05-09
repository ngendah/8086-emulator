//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _MOV_H_
#define _MOV_H_

#include "address.h"
#include "decoders.h"
#include "instruction_templates.h"
#include "io_selectors.h"
#include "micro_op.h"
#include "mov_operators.h"
#include "types.h"

struct Mov : public MicroOp {
  explicit Mov(BUS *bus, Registers *registers) : MicroOp(bus, registers) {}
};

struct MovRegisterRegister : public Mov {
  MovRegisterRegister(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  void before_execute(const Instruction &instruction) override {
    auto mod = instruction.mode_to<mod_reg_rm_t>();
    PLOGD << instruction;
    PLOGD << mod;
    assert(mod.MOD == AddressingModes::MOD::REG);
  }

  MICRO_OP_INSTRUCTION_DCR(MovRegisterRegister, RegisterMovOpTypeSelector,
                           MovOperator, RR_Decoder)
};

struct MovRegisterMemory : public Mov {
  MovRegisterMemory(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  void before_execute(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    assert(opcode.W == 1);
  }

  MICRO_OP_INSTRUCTION_DCR(MovRegisterMemory, WordMovOpTypeSelector,
                           MovOperator, RM_Decoder)
};

struct MovRegisterAndMemory : public Mov {
  MovRegisterAndMemory(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  void execute(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    auto params = MicroOp::Params(_bus, _registers);
    if (mode.MOD == AddressingModes::MOD::REG) {
      return MovRegisterRegister::create(params)->execute(instruction);
    }
    return MovRegisterMemory::create(params)->execute(instruction);
  }

  MICRO_OP_INSTRUCTION(MovRegisterAndMemory)
};

struct MovRegisterImmediate : public Mov {
  MovRegisterImmediate(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(MovRegisterImmediate, ImmediateMovOpTypeSelector,
                           MovOperator, RI_Decoder)
};

struct MovRegisterSegment : public Mov {
  MovRegisterSegment(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  void before_execute(const Instruction &instruction) override {
    PLOGD << instruction;
    auto ins_temp = InstructionTemplate<d_w_t, mod_sr_rm_t>(instruction);
    assert(ins_temp.opcode().W == 0);
    assert(ins_temp.mode().MOD == AddressingModes::MOD::REG);
  }

  MICRO_OP_INSTRUCTION_DCR(MovRegisterSegment, WordMovOpTypeSelector,
                           MovOperator, RS_Decoder)
};

struct MovMemorySegment : public Mov {
  MovMemorySegment(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  void before_execute(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    PLOGD << opcode;
    assert(opcode.W == 0);
  }

  MICRO_OP_INSTRUCTION_DCR(MovMemorySegment, WordMovOpTypeSelector, MovOperator,
                           MS_Decoder)
};

struct MovSegmentAndRegisterMemory : public Mov {
  explicit MovSegmentAndRegisterMemory(BUS *bus, Registers *registers)
      : Mov(bus, registers) {}

  void execute(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    auto params = MicroOp::Params(_bus, _registers);
    if (mode.MOD == AddressingModes::MOD::REG) {
      return MovRegisterSegment::create(params)->execute(instruction);
    }
    return MovMemorySegment::create(params)->execute(instruction);
  }

  MICRO_OP_INSTRUCTION(MovSegmentAndRegisterMemory)
};

struct MovAccumulator : public Mov {
  MovAccumulator(BUS *bus, Registers *registers) : Mov(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(MovAccumulator, WordOrByteMovOpTypeSelector,
                           MovOperator, MA_Decoder)
};

struct MovMemoryImmediate : public Mov {
  explicit MovMemoryImmediate(BUS *bus, Registers *registers)
      : Mov(bus, registers) {}

  MICRO_OP_INSTRUCTION_DCR(MovMemoryImmediate, WordOrByteMovOpTypeSelector,
                           MovOperator, MI_Decoder)
};

#endif // _MOV_H_
