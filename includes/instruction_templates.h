//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef INSTRUCTION_TEMPLATES_H_
#define INSTRUCTION_TEMPLATES_H_

#include "types.h"

typedef struct _flags_t {
  uint8_t _X1 : 4;
  uint8_t O : 1;
  uint8_t D : 1;
  uint8_t I : 1;
  uint8_t T : 1;
  uint8_t S : 1;
  uint8_t Z : 1;
  uint8_t _X2 : 1;
  uint8_t A : 1;
  uint8_t _X3 : 1;
  uint8_t P : 1;
  uint8_t _X4 : 1;
  uint8_t C : 1;

  operator uint16_t() const { return *(uint16_t *)this; }

  friend std::ostream &operator<<(std::ostream &os, const _flags_t &t) {
    os << " flags_t{"
       << fmt::format("O=0b{0:b}, D=0b{1:b}, I=0b{2:b}, T=0b{3:b}, S=0b{4:b}, "
                      "Z=0b{5:b}, A=0b{6:b}, P=0b{7:b}, C=0b{8:b}",
                      t.O, t.D, t.I, t.T, t.S, t.Z, t.A, t.P, t.C)
       << "} ";
    return os;
  }
} flags_t;

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
  operator uint8_t() const { return *(uint8_t *)this; }
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
  operator uint8_t() const { return *(uint8_t *)this; }
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
  operator uint8_t() const { return *(uint8_t *)this; }
} mod_reg_rm_t;

typedef struct _mod_rm_t final {
  uint8_t RM : 3;
  uint8_t _X : 3;
  uint8_t MOD : 2;
  friend std::ostream &operator<<(std::ostream &os, const _mod_rm_t &t) {
    os << " mod_rm_t{"
       << fmt::format("MODE=0x{0:x}(0b{0:b}), _X=0x{1:x}(0b{1:b}), "
                      "RM=0x{2:x}(0b{2:b})",
                      t.MOD, t._X, t.RM)
       << "} ";
    return os;
  }
  operator uint8_t() const { return *(uint8_t *)this; }
} mod_rm_t;

typedef struct _mod_sr_rm_t final {
  uint8_t RM : 3;
  uint8_t SR : 2;
  uint8_t _X : 1;
  uint8_t MOD : 2;
  friend std::ostream &operator<<(std::ostream &os, const _mod_sr_rm_t &t) {
    os << " mod_sr_rm_t{"
       << fmt::format("MODE=0x{0:x}(0b{0:b}), SR=0x0{1:x}(0b0{1:b}), "
                      "RM=0x{2:x}(0b{2:b})",
                      t.MOD, t.SR, t.RM)
       << "} ";
    return os;
  }
  operator uint8_t() const { return *(uint8_t *)this; }
} mod_sr_rm_t;

typedef struct _sr_t final {
  uint8_t _X2 : 3;
  uint8_t SR : 2;
  uint8_t _X1 : 3;
  friend std::ostream &operator<<(std::ostream &os, const _sr_t &t) {
    os << " sr_t{"
       << fmt::format("_X1=0x{0:x}(0b{0:b}), SR=0x{1:x}(0b{1:b}), "
                      "_X2=0x{2:x}(0b{2:b})",
                      t._X1, t.SR, t._X2)
       << "} ";
    return os;
  }
  operator uint8_t() const { return *(uint8_t *)this; }
} sr_t;

typedef struct _d_w_t final {
  uint8_t W : 1;
  uint8_t D : 1;
  uint8_t _X : 6;
  friend std::ostream &operator<<(std::ostream &os, const _d_w_t &t) {
    os << " d_w_t{"
       << fmt::format("_X1=0x{0:x}(0b{0:b}), D=0x{1:x}(0b{1:b}), "
                      "W=0x{2:x}(0b{2:b})",
                      t._X, t.D, t.W)
       << "} ";
    return os;
  }
  operator uint8_t() const { return *(uint8_t *)this; }
} d_w_t;

typedef struct _s_w_t final {
  uint8_t W : 1;
  uint8_t S : 1;
  uint8_t _X : 6;
  friend std::ostream &operator<<(std::ostream &os, const _s_w_t &t) {
    os << " s_w_t{"
       << fmt::format("_X1=0x{0:x}(0b{0:b}), S=0x{1:x}(0b{1:b}), "
                      "W=0x{2:x}(0b{2:b})",
                      t._X, t.S, t.W)
       << "} ";
    return os;
  }
  operator uint8_t() const { return *(uint8_t *)this; }
} s_w_t;

#endif // INSTRUCTION_TEMPLATES_H_
