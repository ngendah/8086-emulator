//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef TYPES_H_
#define TYPES_H_

#include <cstdint>
#include <string>

#define len(x) sizeof(x) * CHAR_BIT
#define UNUSED_PARAM __attribute__((unused))

__attribute__((unused)) typedef void *const void_ptr_t;
typedef uint8_t sop_t; // segment override prefix

__attribute__((unused)) static uint16_t make_word(const uint8_t high,
                                                  const uint8_t low) {
  return ((uint16_t)((high << 8) | (low & 0x00FF)));
}

#pragma GCC diagnostic push
// Allow anon structs
#pragma GCC diagnostic ignored "-Wpedantic"

typedef union _u16 final {
  uint16_t word;
  struct {
    uint8_t lo;
    uint8_t hi;
  };
  _u16(uint16_t val = 0) : word(val) {}
  _u16 operator=(uint16_t val) {
    this->word = val;
    return *this;
  }
  bool operator==(const _u16 val) const { return val.word == word; }
} u16_t;

typedef union _u32 final {
  uint32_t dword;
  struct word {
    u16_t lo;
    u16_t hi;
  };
} u32_t;

typedef u16_t word_t;
typedef u32_t dword_t;

typedef struct _instruction final {
  uint8_t X; // unused
  u16_t imm;
  uint16_t offset;
  u16_t opcode_mod;
  uint8_t sop;
} instruction_t;

#pragma GCC diagnostic pop

#define cast_ui8(x) static_cast<uint8_t>(x)
#define cast_ui16(x) static_cast<uint16_t>(x)
#define sizeof_ui8 sizeof(uint8_t)
#define sizeof_ui16 sizeof(uint16_t)

struct BUS;
struct Registers;

typedef BUS *bus_ptr_t;
typedef Registers *registers_ptr_t;

enum OpTypes {
  word,
  byte,
  high_byte,
  low_byte,
  high_low_byte,
  low_high_byte,
  nop,
};

// NOLINTNEXTLINE
static const std::string _OpTypes[] = {
    "word",          "byte",          "high_byte", "low_byte",
    "high_low_byte", "low_high_byte", "nop"};

enum class CJmpOpTypes {
  noj = 0,
  je = 1,
  jne = 2,
  js = 3,
  jns = 4,
  jo = 5,
  jno = 6,
  jc = 7,
  jb = 8,
  jnae = 9,
  jnc = 10,
  jae = 11,
  jnb = 12,
  jp = 13,
  jpe = 14,
  jnp = 15,
  jpo = 16,
  jbe = 17,
  jna = 18,
  jnbe = 19,
  ja = 20,
  jle = 21,
  jng = 22,
  jnle = 23,
  jg = 24,
  jl = 25,
  jnge = 26,
  jnl = 27,
  jge = 28,
};

// NOLINTNEXTLINE
static const std::string _CJmpOpTypes[] = {
    "noj", "je",  "jne", "js",   "jns", "jo",  "jno",  "jc",  "jb",  "jnae",
    "jnc", "jae", "jnb", "jp",   "jpe", "jnp", "jpo",  "jbe", "jna", "jnbe",
    "ja",  "jle", "jng", "jnle", "jg",  "jl",  "jnge", "jnl", "jge",
};

#endif /* TYPES_H_ */
