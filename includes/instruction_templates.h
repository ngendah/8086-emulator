#ifndef INSTRUCTION_TEMPLATES_H_
#define INSTRUCTION_TEMPLATES_H_

#include "types.h"

typedef struct _opcode_w_t final {
  uint8_t W : 1;
  uint8_t OPCODE : 7;
  operator uint8_t() const { return (uint8_t)(OPCODE << 1 | W); }
  friend std::ostream &operator<<(std::ostream &os, const _opcode_w_t &t) {
    os << " opcode_w_t{"
       << fmt::format("OPCODE=0x{0:x}(0b{0:b}), W=0x{1:x}(0b{1:b})", t.OPCODE,
                      t.W)
       << "} ";
    return os;
  }
} opcode_w_t;

typedef struct _opcode_reg_t final {
  uint8_t REG : 3;
  uint8_t OPCODE : 5;
  operator uint8_t() const { return (uint8_t)(OPCODE << 3 | REG); }
  friend std::ostream &operator<<(std::ostream &os, const _opcode_reg_t &t) {
    os << " opcode_reg_t{"
       << fmt::format("OPCODE=0x{0:x}(0b{0:b}), REG=0x{1:x}(0b{1:b})", t.OPCODE,
                      t.REG)
       << "} ";
    return os;
  }
} opcode_reg_t;

typedef struct _opcode_w_reg_t final {
  uint8_t REG : 3;
  uint8_t W : 1;
  uint8_t OPCODE : 4;
  operator uint8_t() const { return (uint8_t)(OPCODE << 4 | W << 3 | REG); }
  friend std::ostream &operator<<(std::ostream &os, const _opcode_w_reg_t &t) {
    os << " opcode_w_reg_t{"
       << fmt::format("OPCODE=0x{0:x}(0b{0:b}), W=0x{1:x}(0b{1:b}), "
                      "REG=0x{2:x}(0b{2:b})",
                      t.OPCODE, t.W, t.REG)
       << "} ";
    return os;
  }
} opcode_w_reg_t;

#endif // INSTRUCTION_TEMPLATES_H_
