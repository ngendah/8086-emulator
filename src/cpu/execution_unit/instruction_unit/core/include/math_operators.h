//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _MATH_OPERATORS_H_
#define _MATH_OPERATORS_H_

#include "instruction_templates.h"
#include "operators.h"

#include <bitset>

struct WordOrByteWithSignOpTypeSelector : OpTypeSelector {
  OpTypes op_type(const Instruction &instruction) const override {
    return instruction.opcode_to<s_w_t>().S == 1 ? word : byte;
  }
};

struct MathOperator : Operator {
  ~MathOperator() override = default;
  MathOperator(IO *source, IO *destination, OpTypeSelector *selector,
               OpType *optype, Registers *registers)
      : Operator(source, destination, selector, optype, registers) {}

  void execute(const Instruction &instruction) override {
    auto _op_type = _selector->op_type(instruction);
    // NOLINTNEXTLINE
    PLOGD << fmt::format("source_ptr=0x{0:x}, destination_ptr=0x{1:x}",
                         (long)_source, (long)_destination); // NOLINT
    // NOLINTNEXTLINE
    PLOGD << fmt::format("math operation type={}", _OpTypes[_op_type]);
    _op_type_operator->execute(
        OpType::Params(_op_type, _source, _destination, _registers));
  }
};

struct IncrOpType : OpType {
  void execute(const OpType::Params &params) const override {
    auto flags = params._registers->FLAGS.bits<flags_t>();
    memset(&flags, 0, sizeof(flags));
    uint16_t val = params._source->read();
    auto incr_by = params._op_type == OpTypes::word ? 2 : 1;
    flags.C = (val + incr_by) == UINT16_MAX ? 1 : 0;
    flags.O = (val + incr_by) > UINT16_MAX ? 1 : 0;
    val += incr_by;
    params._destination->write(val);
    params._registers->FLAGS.set((uint16_t)flags);
  }
};

// TODO fix to correctly account for negative results
struct DecrOpType : OpType {
  void execute(const OpType::Params &params) const override {
    auto flags = params._registers->FLAGS.bits<flags_t>();
    memset(&flags, 0, sizeof(flags));
    int16_t val = params._source->read();
    auto decr_by = params._op_type == OpTypes::word ? 2 : 1;
    flags.C = 0;
    val -= decr_by;
    flags.Z = val == 0 ? 1 : 0;
    flags.S = val < 0 ? 1 : 0;
    params._destination->write((uint16_t)(flags.S == 1 ? 0 : val));
    params._registers->FLAGS.set((uint16_t)flags);
  }
};

struct CompareOpType : OpType {
  void execute(const OpType::Params &params) const override {
    if (params._op_type == byte)
      byte_cmp(params);
    else
      word_cmp(params);
  }

protected:
  static void byte_cmp(const OpType::Params &params) {
    // TODO revisit and verify it's correct
    auto flags = params._registers->FLAGS.bits<flags_t>();
    memset(&flags, 0, sizeof(flags));
    int8_t l = params._source->read_byte();
    int8_t r = params._destination->read_byte();
    flags.Z = (l - r) > 0 ? 0 : 1;
    flags.S = 0;
    flags.C = r > l ? 1 : 0;
    flags.A = l > r ? 1 : 0;
    flags.O = 0;
    flags.P = std::bitset<8>(l - r).count() % 3 == 0 ? 1 : 0;
    params._registers->FLAGS.set((uint16_t)flags);
  }

  static void word_cmp(const OpType::Params &params) {
    // TODO revisit and verify it's correct
    auto flags = params._registers->FLAGS.bits<flags_t>();
    memset(&flags, 0, sizeof(flags));
    int16_t l = params._source->read();
    int16_t r = params._destination->read();
    flags.Z = (l - r) > 0 ? 0 : 1;
    flags.S = 0;
    flags.C = r > l ? 1 : 0;
    flags.A =
        params._source->read_lo() > params._destination->read_lo() ? 1 : 0;
    flags.O = 0;
    flags.P = std::bitset<16>(l - r).count() % 3 == 0 ? 1 : 0;
    params._registers->FLAGS.set((uint16_t)flags);
  }
};

#endif // _MATH_OPERATORS_H_
