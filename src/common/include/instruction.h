//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "types.h"

struct Instruction final {
  Instruction() = default;

  Instruction(instruction_t instruction);

  Instruction(uint8_t sop, uint16_t opcode_mod);

  Instruction(uint8_t sop, uint16_t opcode_mod, uint16_t offset);

  Instruction(uint8_t sop, uint16_t opcode_mod, uint16_t offset, uint16_t data);

  Instruction(uint8_t sop, uint16_t opcode_mod, uint16_t offset, uint8_t port);

  Instruction(const Instruction &rhs);

  Instruction(Instruction &rhs);

  ~Instruction() = default;

  Instruction sop(uint8_t sop);

  Instruction opcode(uint8_t opcode);

  Instruction mode(uint8_t mod_reg);

  Instruction offset(int16_t offset);

  Instruction data(uint16_t data);

  Instruction port(uint8_t port);

  operator sop_t() const;

  uint8_t sop() const;

  template <typename T> T opcode_to() const {
    return *(T *)&_instruction.opcode_mod.hi;
  }

  template <typename T> T mode_to() const {
    return *(T *)&_instruction.opcode_mod.lo;
  }

  uint16_t offset() const;

  template <typename T> T data() const { return (T)_instruction.imm.word; }

  uint8_t port() const;

  friend std::ostream &operator<<(std::ostream &ostream,
                                  const Instruction &instruction);

private:
  instruction_t _instruction;
};

template <typename T, typename P> struct InstructionTemplate final {
  InstructionTemplate() = default;

  InstructionTemplate(const Instruction &val) : _instruction(val) {}

  InstructionTemplate(const InstructionTemplate &rhs)
      : _instruction(rhs._instruction) {}

  ~InstructionTemplate() = default;

  InstructionTemplate &operator=(const InstructionTemplate &rhs);

  T opcode() const { return _instruction.opcode_to<T>(); }

  P mode() const { return _instruction.mode_to<P>(); }

  friend std::ostream &
  operator<<(std::ostream &ostream,
             const InstructionTemplate<T, P> &instruction_template) {
    ostream << instruction_template._instruction;
    return ostream;
  }

private:
  Instruction _instruction;
};

#endif // _INSTRUCTION_H_
