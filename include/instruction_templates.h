//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef INSTRUCTION_TEMPLATES_H_
#define INSTRUCTION_TEMPLATES_H_

#include "types.h"

/**
 *
 * Instruction templates give meaning to a byte or a word.
 *
 * An 8086 instruction is broken into;
 * 1. a mandatory 8-bit opcode,
 * 2. an 8-bit mode and
 * 3. 'data' which includes offsets, immediate values e.tc
 *
 * What each bit of an opcode or mode represent depends
 * on the instruction but there are a common patterns,
 * and its this common patterns that are captured here as templates.
 */

typedef struct _flags_t {
  uint8_t X1 : 4;
  uint8_t O : 1;
  uint8_t D : 1;
  uint8_t I : 1;
  uint8_t T : 1;
  uint8_t S : 1;
  uint8_t Z : 1;
  uint8_t X2 : 1;
  uint8_t A : 1;
  uint8_t X3 : 1;
  uint8_t P : 1;
  uint8_t X4 : 1;
  uint8_t C : 1;

  operator uint16_t() { return *reinterpret_cast<uint16_t *>(this); } // NOLINT

  friend std::ostream &operator<<(std::ostream &ostream,
                                  const _flags_t &flags) {
    ostream << " flags_t{"
            << fmt::format(
                   "O=0b{0:b}, D=0b{1:b}, I=0b{2:b}, T=0b{3:b}, S=0b{4:b}, "
                   "Z=0b{5:b}, A=0b{6:b}, P=0b{7:b}, C=0b{8:b}",
                   flags.O, flags.D, flags.I, flags.T, flags.S, flags.Z,
                   flags.A, flags.P, flags.C)
            << "} ";
    return ostream;
  }
} flags_t;

typedef struct _opcode_w_t final {
  uint8_t W : 1;
  uint8_t OPCODE : 7;
  friend std::ostream &operator<<(std::ostream &ostream,
                                  const _opcode_w_t &opcode_w) {
    ostream << " opcode_w{"
            << fmt::format("OPCODE=0x{0:x}(0b{0:b}), W=0x{1:x}(0b{1:b})",
                           opcode_w.OPCODE, opcode_w.W)
            << "} ";
    return ostream;
  }
} opcode_w_t;

typedef struct _opcode_reg_t final {
  uint8_t REG : 3;
  uint8_t OPCODE : 5;
  friend std::ostream &operator<<(std::ostream &ostream,
                                  const _opcode_reg_t &opcode_reg) {
    ostream << " opcode_reg_t{"
            << fmt::format("OPCODE=0x{0:x}(0b{0:b}), REG=0x{1:x}(0b{1:b})",
                           opcode_reg.OPCODE, opcode_reg.REG)
            << "} ";
    return ostream;
  }
  operator uint8_t() { return *reinterpret_cast<uint8_t *>(this); } // NOLINT

} opcode_reg_t;

typedef struct _opcode_w_reg_t final {
  uint8_t REG : 3;
  uint8_t W : 1;
  uint8_t OPCODE : 4;
  friend std::ostream &operator<<(std::ostream &ostream,
                                  const _opcode_w_reg_t &opcode_w_reg) {
    ostream << " opcode_w_reg_t{"
            << fmt::format("OPCODE=0x{0:x}(0b{0:b}), W=0x{1:x}(0b{1:b}), "
                           "REG=0x{2:x}(0b{2:b})",
                           opcode_w_reg.OPCODE, opcode_w_reg.W,
                           opcode_w_reg.REG)
            << "} ";
    return ostream;
  }
  operator uint8_t() { return *reinterpret_cast<uint8_t *>(this); } // NOLINT

} opcode_w_reg_t;

typedef struct _mod_reg_rm_t final {
  uint8_t RM : 3;
  uint8_t REG : 3;
  uint8_t MOD : 2;
  friend std::ostream &operator<<(std::ostream &ostream,
                                  const _mod_reg_rm_t &mod_reg_rm) {
    ostream << " mod_reg_rm_t{"
            << fmt::format("MODE=0x{0:x}(0b{0:b}), REG=0x{1:x}(0b{1:b}), "
                           "RM=0x{2:x}(0b{2:b})",
                           mod_reg_rm.MOD, mod_reg_rm.REG, mod_reg_rm.RM)
            << "} ";
    return ostream;
  }
  operator uint8_t() { return *reinterpret_cast<uint8_t *>(this); } // NOLINT

} mod_reg_rm_t;

typedef struct _mod_rm_t final {
  uint8_t RM : 3;
  uint8_t _X : 3;
  uint8_t MOD : 2;
  friend std::ostream &operator<<(std::ostream &ostream,
                                  const _mod_rm_t &mod_rm) {
    ostream << " mod_rm_t{"
            << fmt::format("MODE=0x{0:x}(0b{0:b}), _X=0x{1:x}(0b{1:b}), "
                           "RM=0x{2:x}(0b{2:b})",
                           mod_rm.MOD, mod_rm._X, mod_rm.RM)
            << "} ";
    return ostream;
  }
  operator uint8_t() { return *reinterpret_cast<uint8_t *>(this); } // NOLINT

} mod_rm_t;

typedef struct _mod_sr_rm_t final {
  uint8_t RM : 3;
  uint8_t SR : 2;
  uint8_t X : 1;
  uint8_t MOD : 2;
  friend std::ostream &operator<<(std::ostream &ostream,
                                  const _mod_sr_rm_t &mod_sr_rm) {
    ostream << " mod_sr_rm_t{"
            << fmt::format("MODE=0x{0:x}(0b{0:b}), SR=0x0{1:x}(0b0{1:b}), "
                           "RM=0x{2:x}(0b{2:b})",
                           mod_sr_rm.MOD, mod_sr_rm.SR, mod_sr_rm.RM)
            << "} ";
    return ostream;
  }
  operator uint8_t() { return *reinterpret_cast<uint8_t *>(this); } // NOLINT

} mod_sr_rm_t;

typedef struct _sr_t final {
  uint8_t X2 : 3;
  uint8_t SR : 2;
  uint8_t X1 : 3;
  // NOLINTNEXTLINE
  friend std::ostream &operator<<(std::ostream &ostream, const _sr_t &sr) {
    ostream << " sr_t{"
            << fmt::format("_X1=0x{0:x}(0b{0:b}), SR=0x{1:x}(0b{1:b}), "
                           "_X2=0x{2:x}(0b{2:b})",
                           sr.X1, sr.SR, sr.X2)
            << "} ";
    return ostream;
  }
  operator uint8_t() { return *reinterpret_cast<uint8_t *>(this); } // NOLINT

} sr_t;

typedef struct _d_w_t final {
  uint8_t W : 1;
  uint8_t D : 1;
  uint8_t X : 6;
  // NOLINTNEXTLINE
  friend std::ostream &operator<<(std::ostream &ostream, const _d_w_t &dw) {
    ostream << " d_w_t{"
            << fmt::format("_X1=0x{0:x}(0b{0:b}), D=0x{1:x}(0b{1:b}), "
                           "W=0x{2:x}(0b{2:b})",
                           dw.X, dw.D, dw.W)
            << "} ";
    return ostream;
  }
  operator uint8_t() { return *reinterpret_cast<uint8_t *>(this); } // NOLINT

} d_w_t;

typedef struct _s_w_t final {
  uint8_t W : 1;
  uint8_t S : 1;
  uint8_t X : 6;
  // NOLINTNEXTLINE
  friend std::ostream &operator<<(std::ostream &ostream, const _s_w_t &sw) {
    ostream << " s_w_t{"
            << fmt::format("_X1=0x{0:x}(0b{0:b}), S=0x{1:x}(0b{1:b}), "
                           "W=0x{2:x}(0b{2:b})",
                           sw.X, sw.S, sw.W)
            << "} ";
    return ostream;
  }
  operator uint8_t() { return *reinterpret_cast<uint8_t *>(this); } // NOLINT

} s_w_t;

#endif // INSTRUCTION_TEMPLATES_H_
