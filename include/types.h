//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef TYPES_H_
#define TYPES_H_

#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <memory>
#include <utility>

#define len(x) sizeof(x) * CHAR_BIT
#define UNUSED_PARAM __attribute__((unused))

__attribute__((unused)) typedef void *const void_ptr_t;
typedef uint8_t sop_t; // segment override prefix

__attribute__((unused)) static uint16_t make_word(const uint8_t high, const uint8_t low) {
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
  _u16 operator=(uint16_t val){
    this->word = val;
    return *this;
  }
} u16_t;

typedef struct _instruction final {
  uint8_t X; // unused
  u16_t imm;
  uint16_t offset;
  u16_t opcode_mod;
  uint8_t sop;
} instruction_t;

#pragma GCC diagnostic pop

struct Bytes;
struct IO;
struct BUS;
struct Instruction;

struct IO {
  IO() = default;

  virtual ~IO() = default;

  virtual void write_hi(uint8_t val) = 0;

  virtual void write_lo(uint8_t val) = 0;

  virtual void write(uint8_t val) = 0;

  virtual void write(uint16_t val) = 0;

  virtual uint16_t read() const = 0;

  virtual uint8_t read_byte() const = 0;

  virtual uint8_t read_hi() const = 0;

  virtual uint8_t read_lo() const = 0;
};

struct Instruction final {
  Instruction() = default;

  Instruction(instruction_t instruction) : _instruction(instruction) {}

  Instruction(uint8_t sop, uint16_t opcode_mod) {
    _instruction.sop = sop;
    _instruction.opcode_mod = opcode_mod;
  }

  Instruction(uint8_t sop, uint16_t opcode_mod, uint16_t offset) {
    _instruction.sop = sop;
    _instruction.opcode_mod = opcode_mod;
    _instruction.offset = offset;
  }

  Instruction(uint8_t sop, uint16_t opcode_mod, uint16_t offset,
              uint16_t data) {
    _instruction.sop = sop;
    _instruction.opcode_mod = opcode_mod;
    _instruction.offset = offset;
    _instruction.imm = data;
  }

  Instruction(uint8_t sop, uint16_t opcode_mod, uint16_t offset,
              uint8_t port) {
    _instruction.sop = sop;
    _instruction.opcode_mod = opcode_mod;
    _instruction.offset = offset;
    _instruction.imm.lo = port;
    _instruction.imm.hi = 0;
  }

  Instruction(const Instruction &rhs) : _instruction(rhs._instruction) {}

  Instruction(Instruction &rhs) : _instruction(rhs._instruction) {}

  ~Instruction() = default;

  Instruction sop(uint8_t sop) {
    _instruction.sop = sop;
    return *this;
  }

  Instruction opcode(uint8_t opcode) {
    _instruction.opcode_mod.hi = opcode;
    return *this;
  }

  Instruction mode(uint8_t mod_reg) {
    _instruction.opcode_mod.lo = mod_reg;
    return *this;
  }

  Instruction offset(int16_t offset) {
    _instruction.offset = offset;
    return *this;
  }

  Instruction data(uint16_t data) {
    _instruction.imm = data;
    return *this;
  }

  Instruction port(uint8_t port) {
    _instruction.imm.lo = port;
    return *this;
  }

  operator sop_t() const { return _instruction.sop; }

  uint8_t sop() const { return _instruction.sop; }

  template <typename T> T opcode_to() const {
    return *(T *)&_instruction.opcode_mod.hi;
  }

  template <typename T> T mode_to() const {
    return *(T *)&_instruction.opcode_mod.lo;
  }

  uint16_t offset() const { return _instruction.offset; }

  template <typename T> T data() const { return (T)_instruction.imm.word; }

  uint8_t port() const { return _instruction.imm.lo; }

  friend std::ostream &operator<<(std::ostream &ostream,
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
private:
  instruction_t _instruction;
};

template <typename T, typename P> struct InstructionTemplate final {
  InstructionTemplate() = default;

  InstructionTemplate(const Instruction &val) : _instruction(val) {}

  InstructionTemplate(const InstructionTemplate &rhs)
      : _instruction(rhs._instruction) {}

  ~InstructionTemplate() = default;

  InstructionTemplate &operator=(const InstructionTemplate &rhs) {
    _instruction = rhs._instruction;
    return *this;
  };

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

class Address final {
protected:
  uint32_t _address;

public:
  Address() : _address(0) {}
  Address(const Address &rhs) : _address(rhs._address) {}
  Address(const uint8_t address) : _address(address) {}
  Address(const uint16_t address) : _address(address) {}
  Address(const uint32_t address) : _address(address) {}
  Address &operator=(const Address &rhs) {
    _address = rhs._address;
    return *this;
  }
  operator uint8_t() const { return (uint8_t)_address & 0x000000ff; }
  operator uint16_t() const { return (uint16_t)_address & 0x0000ffff; }
  operator uint32_t() const { return (uint32_t)_address & 0x000fffff; }

  Address &operator+=(uint16_t offset) {
    _address += offset;
    return *this;
  }

  Address operator+(uint16_t offset) const {
    return Address(_address + offset);
  }

  Address operator+(const Address &rhs) const {
    return Address(_address + rhs._address);
  }
};

struct Flags final : IO {
  Flags() : _flags(0) {}

  Flags(const Flags &rhs) : _flags(rhs._flags) {}

  void set(uint16_t flags) { _flags = flags; }

  template <typename T> T bits() const { return *(T *)&_flags.word; }

  operator uint16_t() const { return _flags.word; }

  friend std::ostream &operator<<(UNUSED_PARAM std::ostream &ostream,
                                  UNUSED_PARAM const Flags &) {
    // instead do: os << flags.bits<flags_t>();
    assert(false);
    // return ostream;
  }

  void write_hi(const uint8_t val) override { _flags.hi = val; }

  void write_lo(const uint8_t val) override { _flags.lo = val; }

  void write(const uint16_t val) override { _flags.word = val; }

  uint16_t read() const override { return _flags.word; }

  uint8_t read_hi() const override { return _flags.hi; }

  uint8_t read_lo() const override { return _flags.lo; }

protected:
  void write(UNUSED_PARAM const uint8_t) override { assert(false); }

  uint8_t read_byte() const override {
    assert(false);
    // return (uint8_t)_flags.lo;
  }

  u16_t _flags;
};

struct BUS {
  virtual ~BUS() = default;
  virtual uint16_t write(Address const *, const Bytes &) = 0;
  virtual Bytes read(Address const *, uint16_t size) = 0;
  virtual std::streambuf *rdbuf() { return nullptr; }
};

class Register : public IO {
protected:
  u16_t _register;
  std::string _name;

public:
  explicit Register(uint16_t val = 0) : _register(val) {}

  Register(const std::string &name) : _register(0), _name(name) {}

  Register(const Register &rhs) : _register(rhs._register), _name(rhs._name) {}

  ~Register() override = default;

  void write_hi(const uint8_t val) override { _register.hi = val; }

  void write_lo(const uint8_t val) override { _register.lo = val; }

  void write(const uint16_t val) override { _register.word = val; }

  void write(const uint8_t val) override { _register.lo = val; }

  uint16_t read() const override { return _register.word; }

  uint8_t read_hi() const override { return _register.hi; }

  uint8_t read_lo() const override { return _register.lo; }

  uint8_t read_byte() const override { return _register.lo; }

  operator uint16_t() const { return _register.word; }

  Register &operator+=(uint16_t offset) {
    _register.word += offset;
    return *this;
  }

  Register &operator-=(uint16_t offset) {
    _register.word -= offset;
    return *this;
  }

  virtual Register &operator=(uint16_t val) {
    _register.word = val;
    return *this;
  }

  std::string name() const { return _name; }

  friend std::ostream &operator<<(std::ostream &ostream, const Register &rhs) {
    ostream << fmt::format("{0:}=0x{1:x}", rhs._name, rhs._register.word);
    return ostream;
  }
};

struct Segment final : public Register {
  explicit Segment(uint16_t val = 0) : Register(val){};

  Segment(const Segment &rhs) : Register(rhs) {}

  Segment(const std::string& name) : Register(name) {}

  ~Segment() override = default;

  Segment &operator=(uint16_t val) override {
    _register.word = val;
    return *this;
  }

  Address address(const Register &reg, uint16_t offset = 0x0) {
    uint16_t eff_addr = offset + (uint16_t)reg;
    return Address((uint32_t)(((uint32_t) * this) * 0x10 + eff_addr));
  }

  Address address(uint16_t eff_addr) {
    return Address((uint32_t)(((uint32_t) * this) * 0x10 + eff_addr));
  }
};

struct Registers final {
  Register AX, BX, CX, DX, SP, BP, SI, DI, IP;
  Segment CS, DS, SS, ES;
  Flags FLAGS;
  Registers()
      : AX("AX"), BX("BX"), CX("CX"), DX("DX"), SP("SP"), BP("BP"), SI("SI"),
        DI("DI"), IP("IP"), CS("CS"), DS("DS"), SS("SS"), ES("ES") {}
};

typedef BUS *bus_ptr_t;             // TODO refactor to BUS *const
typedef Registers *registers_ptr_t; // TODO refactor to Registers *const

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
