//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _MICRO_OP_H_
#define _MICRO_OP_H_

#include "decoder.h"
#include "instruction.h"
#include "registers.h"
#include "types.h"

#include <fmt/core.h>
#include <iostream>
#include <memory>

struct MicroOp {

  struct ExecuteStrategy {
    virtual ~ExecuteStrategy() = default;
    ExecuteStrategy(Decoder *const decoder, MicroOp *const micro_op)
        : _decoder(decoder), _micro_op(micro_op) {}

    virtual void execute(const Instruction &instruction) const = 0;

    Decoder *_decoder;
    MicroOp *_micro_op;
  };

  struct Params {
    BUS *bus;
    Registers *registers;
    Params(BUS *bus = nullptr, Registers *registers = nullptr)
        : bus(bus), registers(registers) {}
  };

  struct Key {
    uint8_t _opcode;
    uint8_t _mask;

    Key(uint8_t opcode = 0x0, uint8_t mask = 0xFF)
        : _opcode(opcode), _mask(mask) {}

    Key(const Key &rhs) : _opcode(rhs._opcode), _mask(rhs._mask) {}

    bool operator<(const Key &rhs) const { return _opcode < (rhs & _mask); }

    uint8_t operator&(uint8_t mask) const { return _opcode & mask; }

    bool operator==(const Key &rhs) const { return _opcode == (rhs & _mask); }

    friend std::ostream &operator<<(std::ostream &ostream, const Key &key) {
      ostream << fmt::format("opcode=0x{:x}", key._opcode);
      return ostream;
    }
  };

  MicroOp(bus_ptr_t bus = nullptr, registers_ptr_t registers = nullptr)
      : _bus(bus), _registers(registers) {}

  MicroOp(const MicroOp &rhs) : _bus(rhs._bus), _registers(rhs._registers) {}

  virtual ~MicroOp() = default;

  virtual void execute(const Instruction &) = 0;

  virtual Instruction before_decode(const Instruction &instruction) {
    return instruction;
  }

  virtual void before_execute(UNUSED_PARAM const Instruction &instruction) {}

  virtual void after_execute(UNUSED_PARAM const Instruction &instruction) {}

  bus_ptr_t _bus;
  registers_ptr_t _registers;

protected:
  template <class OpTypeSelectorT, class OperatorT>
  struct ExecuteStrategy2 final : ExecuteStrategy {
    ExecuteStrategy2(Decoder *const decoder, MicroOp *const micro_op)
        : ExecuteStrategy(decoder, micro_op) {}

    void execute(const Instruction &instruction) const override {
      auto op_selector = OpTypeSelectorT();
      auto _instruction = _micro_op->before_decode(instruction);
      auto src_dest = _decoder->decode(_instruction);
      _micro_op->before_execute(_instruction);
      auto uop_operator =
          OperatorT(src_dest.first, src_dest.second, &op_selector);
      uop_operator.execute(_instruction);
      _micro_op->after_execute(_instruction);
    }
  };

  template <class OpTypeSelectorT, class OperatorT, class OpTypeT>
  struct ExecuteStrategy21 final : ExecuteStrategy {
    ExecuteStrategy21(Decoder *const decoder, MicroOp *const micro_op)
        : ExecuteStrategy(decoder, micro_op) {}

    void execute(const Instruction &instruction) const override {
      auto op_selector = OpTypeSelectorT();
      auto op_type = OpTypeT();
      auto _instruction = _micro_op->before_decode(instruction);
      _micro_op->before_execute(_instruction);
      auto uop_operator =
          OperatorT(src_dest.first, src_dest.second, &op_selector, &op_type,
                    _micro_op->_registers);
      uop_operator.execute(_instruction);
      _micro_op->after_execute(_instruction);
    }
  };

  struct Executor final {
    Executor(ExecuteStrategy *const execute_strategy)
        : _execute_strategy(execute_strategy) {}

    void execute(const Instruction &instruction) const {
      _execute_strategy->execute(instruction);
    }

    ExecuteStrategy *_execute_strategy;
  };
};

#define MICRO_OP_INSTRUCTION(cls)                                              \
  static std::shared_ptr<MicroOp> create(const MicroOp::Params &params) {      \
    PLOGD << #cls << "::create";                                               \
    return std::make_shared<cls>(params.bus, params.registers);                \
  }

#define MICRO_OP_INSTRUCTION_D(cls, execute_strategy_cls, decoder_cls)         \
  static std::shared_ptr<MicroOp> create(const MicroOp::Params &params) {      \
    PLOGD << #cls << "::create";                                               \
    return std::make_shared<cls>(params.bus, params.registers);                \
  }                                                                            \
  void execute(const Instruction &instruction) override {                      \
    auto decoder = decoder_cls(_bus, _registers);                              \
    auto _strategy = execute_strategy_cls(&decoder, (MicroOp *)this);          \
    Executor(&_strategy).execute(instruction);                                 \
  }

#define MICRO_OP_INSTRUCTION_DCR(cls, op_type_selector_cls, operator_type_cls, \
                                 decoder_cls)                                  \
  static std::shared_ptr<MicroOp> create(const MicroOp::Params &params) {      \
    PLOGD << #cls << "::create";                                               \
    return std::make_shared<cls>(params.bus, params.registers);                \
  }                                                                            \
  void execute(const Instruction &instruction) override {                      \
    auto decoder = decoder_cls(_bus, _registers);                              \
    auto _strategy =                                                           \
        ExecuteStrategy2<op_type_selector_cls, operator_type_cls>(             \
            &decoder, (MicroOp *)this);                                        \
    Executor(&_strategy).execute(instruction);                                 \
  }

#define MICRO_OP_INSTRUCTION_DCRE(cls, op_type_selector_cls,                   \
                                  operator_type_cls, op_type_cls, decoder_cls) \
  static std::shared_ptr<MicroOp> create(const MicroOp::Params &params) {      \
    PLOGD << #cls << "::create";                                               \
    return std::make_shared<cls>(params.bus, params.registers);                \
  }                                                                            \
  void execute(const Instruction &instruction) override {                      \
    auto decoder = decoder_cls(_bus, _registers);                              \
    auto _strategy =                                                           \
        ExecuteStrategy21<op_type_selector_cls, operator_type_cls,             \
                          op_type_cls>(&decoder, (MicroOp *)this);             \
    Executor(&_strategy).execute(instruction);                                 \
  }

#endif // _MICRO_OP_H_
