#ifndef _MOV_H_
#define _MOV_H_

#include <array>

#include "instruction_templates.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct IOReader {
  virtual IO *reader(const Instruction &) = 0;
};

struct IOWriter {
  virtual IO *writer(const Instruction &) = 0;
};

class MovRegisterRegister : RegisterMovOpTypeSelector {
public:
  explicit MovRegisterRegister(Registers *registers) : _registers(registers) {}

  void execute(const Instruction &instruction) {
    auto mod = instruction.mode_to<mod_reg_rm_t>();
    PLOGD << instruction;
    PLOGD << mod;
    assert(mod.MOD == 0x3);
    return mov(instruction);
  }

protected:
  Registers *_registers;

  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct _RegisterSelector2 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().RM;
    }
  };

  struct _IOReader final : IOReader, _RegisterSelector1, _RegisterSelector2 {
    Registers *_registers;
    std::array<RegisterSelector *, 2> _selectors;

    explicit _IOReader(Registers *registers) : _registers(registers) {
      _selectors = {
          (_RegisterSelector1 *)this,
          (_RegisterSelector2 *)this,
      };
    }

    IO *reader(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return RegisterIOSelector(_registers, _selectors[opcode.D])
          .get(instruction);
    }
  };

  struct _IOWriter final : IOWriter, _RegisterSelector1, _RegisterSelector2 {
    Registers *_registers;
    std::array<RegisterSelector *, 2> _selectors;

    explicit _IOWriter(Registers *registers) : _registers(registers) {
      _selectors = {
          (_RegisterSelector2 *)this,
          (_RegisterSelector1 *)this,
      };
    }

    IO *writer(const Instruction &instruction) override {
      auto opcode = instruction.opcode_to<d_w_t>();
      return RegisterIOSelector(_registers, _selectors[opcode.D])
          .get(instruction);
    }
  };

  void mov(const Instruction &instruction) {
    auto mov_operator =
        MovOperator(_IOReader(_registers).reader(instruction),
                    _IOWriter(_registers).writer(instruction), this);
    mov_operator.mov(instruction);
  }
};

class MovRegisterImmediate : public ImmediateMovOpTypeSelector {
public:
  explicit MovRegisterImmediate(Registers *registers) : _registers(registers) {}

  void execute(const Instruction &instruction) {
    return MovOperator(_IOReader().reader(instruction),
                       _IOWriter(_registers).writer(instruction), this)
        .mov(instruction);
  }

protected:
  Registers *_registers;

  struct _RegisterSelector : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.opcode_to<opcode_w_reg_t>().REG;
    }
  };

  struct _IOReader : IOReader {
    ValueIO _value_io;
    explicit _IOReader() = default;

    IO *reader(const Instruction &instruction) override {
      auto w_reg = instruction.opcode_to<opcode_w_reg_t>();
      if (w_reg.W == 1)
        _value_io = instruction.data<uint16_t>();
      else
        _value_io = instruction.data<uint8_t>();
      return &_value_io;
    }
  };

  struct _IOWriter : IOWriter, _RegisterSelector {
    Registers *_registers;

    explicit _IOWriter(Registers *registers) : _registers(registers) {}

    IO *writer(const Instruction &instruction) override {
      return RegisterIOSelector(_registers, this).get(instruction);
    }
  };
};

#endif
