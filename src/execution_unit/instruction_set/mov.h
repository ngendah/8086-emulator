#ifndef _MOV_H_
#define _MOV_H_

#include <array>

#include "instruction_templates.h"
#include "io_selectors.h"
#include "types.h"

struct IOReader {
  virtual IO *reader(const Instruction &) = 0;
};

struct IOWriter {
  virtual IO *writer(const Instruction &) = 0;
};

class MovRegisterRegister final {
public:
  explicit MovRegisterRegister(Registers *registers) : _registers(registers) {}

  void execute(const Instruction &instruction) {
    auto mod = instruction.opcode_to<mod_reg_rm_t>();
    assert(mod.MOD == 0x3);
  }

protected:
  Registers *_registers;

  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct _RegisterSelector2 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) {
      return instruction.mode_to<mod_reg_rm_t>().RM;
    }
  };

  struct _IOReader : IOReader, _RegisterSelector1, _RegisterSelector2 {
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

  struct _IOWriter : IOWriter, _RegisterSelector1, _RegisterSelector2 {
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

  void mov(const Instruction &instruction) {}
};

#endif
