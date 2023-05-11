#include "instruction.h"

#include <fmt/core.h>
#include <iostream>

Instruction::Instruction(instruction_t instruction)
    : _instruction(instruction) {}

Instruction::Instruction(uint8_t sop, uint16_t opcode_mod) {
  _instruction.sop = sop;
  _instruction.opcode_mod = opcode_mod;
}

Instruction::Instruction(uint8_t sop, uint16_t opcode_mod, uint16_t offset) {
  _instruction.sop = sop;
  _instruction.opcode_mod = opcode_mod;
  _instruction.offset = offset;
}

Instruction::Instruction(uint8_t sop, uint16_t opcode_mod, uint16_t offset,
                         uint16_t data) {
  _instruction.sop = sop;
  _instruction.opcode_mod = opcode_mod;
  _instruction.offset = offset;
  _instruction.imm = data;
}

Instruction::Instruction(uint8_t sop, uint16_t opcode_mod, uint16_t offset,
                         uint8_t port) {
  _instruction.sop = sop;
  _instruction.opcode_mod = opcode_mod;
  _instruction.offset = offset;
  _instruction.imm.lo = port;
  _instruction.imm.hi = 0;
}

Instruction::Instruction(const Instruction &rhs)
    : _instruction(rhs._instruction) {}

Instruction::Instruction(Instruction &rhs) : _instruction(rhs._instruction) {}

Instruction Instruction::sop(uint8_t sop) {
  _instruction.sop = sop;
  return *this;
}

Instruction Instruction::opcode(uint8_t opcode) {
  _instruction.opcode_mod.hi = opcode;
  return *this;
}

Instruction Instruction::mode(uint8_t mod_reg) {
  _instruction.opcode_mod.lo = mod_reg;
  return *this;
}

Instruction Instruction::offset(int16_t offset) {
  _instruction.offset = offset;
  return *this;
}

Instruction Instruction::data(uint16_t data) {
  _instruction.imm = data;
  return *this;
}

Instruction Instruction::port(uint8_t port) {
  _instruction.imm.lo = port;
  return *this;
}

Instruction::operator sop_t() const { return _instruction.sop; }

uint8_t Instruction::sop() const { return _instruction.sop; }

uint16_t Instruction::offset() const { return _instruction.offset; }

uint8_t Instruction::port() const { return _instruction.imm.lo; }

std::ostream &operator<<(std::ostream &ostream,
                         const Instruction &instruction) {
  ostream << " "
          << fmt::format("sop=0x{0:x}, opcode=0x{1:x}(0b{1:b}), mode="
                         "0x{2:x}(0b{2:b}), offset=0x{3:x}, data=0x{4:x}",
                         instruction._instruction.sop,
                         instruction._instruction.opcode_mod.hi,
                         instruction._instruction.opcode_mod.lo,
                         instruction._instruction.offset,
                         instruction._instruction.imm.word)
          << " ";
  return ostream;
}

template <typename T, typename P>
InstructionTemplate<T, P> &
InstructionTemplate<T, P>::operator=(const InstructionTemplate &rhs) {
  _instruction = rhs._instruction;
  return *this;
};
