#ifndef INSTRUCTION_TEMPLATES_H_
#define INSTRUCTION_TEMPLATES_H_

#include "types.h"

typedef struct _opcode_w_t final {
  uint8_t W : 1;
  uint8_t OPCODE : 7;
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
  friend std::ostream &operator<<(std::ostream &os, const _opcode_w_reg_t &t) {
    os << " opcode_w_reg_t{"
       << fmt::format("OPCODE=0x{0:x}(0b{0:b}), W=0x{1:x}(0b{1:b}), "
                      "REG=0x{2:x}(0b{2:b})",
                      t.OPCODE, t.W, t.REG)
       << "} ";
    return os;
  }
} opcode_w_reg_t;

typedef struct _mod_reg_rm_t final {
  uint8_t RM : 3;
  uint8_t REG : 3;
  uint8_t MOD : 2;
  friend std::ostream &operator<<(std::ostream &os, const _mod_reg_rm_t &t) {
    os << " mod_reg_rm_t{"
       << fmt::format("MODE=0x{0:x}(0b{0:b}), REG=0x{1:x}(0b{1:b}), "
                      "RM=0x{2:x}(0b{2:b})",
                      t.MOD, t.REG, t.RM)
       << "} ";
    return os;
  }
} mod_reg_rm_t;

typedef struct _sr_t final {
  uint8_t _X2 : 3;
  uint8_t SR : 2;
  uint8_t _X1 : 3;
  friend std::ostream &operator<<(std::ostream &os, const _mod_reg_rm_t &t) {
    os << " sr_t{"
       << fmt::format("_X1=0x{0:x}(0b{0:b}), SR=0x{1:x}(0b{1:b}), "
                      "_X2=0x{2:x}(0b{2:b})",
                      t.MOD, t.REG, t.RM)
       << "} ";
    return os;
  }
} sr_t;

typedef struct _d_w_t final {
  uint8_t W : 1;
  uint8_t D : 1;
  uint8_t _X : 3;
  friend std::ostream &operator<<(std::ostream &os, const _d_w_t &t) {
    os << " sr_t{"
       << fmt::format("_X1=0x{0:x}(0b{0:b}), D=0x{1:x}(0b{1:b}), "
                      "W=0x{2:x}(0b{2:b})",
                      t._X, t.D, t.W)
       << "} ";
    return os;
  }
} d_w_t;

#endif // INSTRUCTION_TEMPLATES_H_
