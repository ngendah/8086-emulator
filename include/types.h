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

static uint16_t make_word(const uint8_t high, const uint8_t low) {
  return ((uint16_t)((high << 8) | (low & 0x00FF)));
}

#pragma GCC diagnostic push
// Allow anon structs
#pragma GCC diagnostic ignored "-Wpedantic"

typedef union _u16 {
  uint16_t word;
  struct {
    uint8_t lo;
    uint8_t hi;
  };
  _u16(uint16_t val = 0) : word(val) {}
} u16_t;

typedef struct _instruction {
  uint8_t X; // reserved
  union __imm {
    uint16_t data;
    struct {
      uint8_t lo; // also port
      uint8_t hi;
    };
  } _imm;
  uint16_t _offset;
  union __opcode_mode {
    uint16_t word;
    struct {
      uint8_t mode;
      uint8_t opcode;
    };
  } _opcode_mode;
  uint8_t _sop;
  _instruction() : _sop(0xff) {}
} instruction_t;

#pragma GCC diagnostic pop

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

class Instruction final {
  instruction_t _instruction;

public:
  Instruction() = default;

  Instruction(instruction_t instruction) : _instruction(instruction) {}

  Instruction(uint8_t sop, uint16_t opcode_mode) {
    _instruction._sop = sop;
    _instruction._opcode_mode.word = opcode_mode;
  }

  Instruction(uint8_t sop, uint16_t opcode_mode, uint16_t offset) {
    _instruction._sop = sop;
    _instruction._opcode_mode.word = opcode_mode;
    _instruction._offset = offset;
  }

  Instruction(uint8_t sop, uint16_t opcode_mode, uint16_t offset,
              uint16_t data) {
    _instruction._sop = sop;
    _instruction._opcode_mode.word = opcode_mode;
    _instruction._offset = offset;
    _instruction._imm.data = data;
  }

  Instruction(uint8_t sop, uint16_t opcode_mode, uint16_t offset,
              uint8_t port) {
    _instruction._sop = sop;
    _instruction._opcode_mode.word = opcode_mode;
    _instruction._offset = offset;
    _instruction._imm.lo = port;
  }

  Instruction(const Instruction &rhs) : _instruction(rhs._instruction) {}

  Instruction(Instruction &rhs) : _instruction(rhs._instruction) {}

  ~Instruction() = default;

  Instruction sop(uint8_t sop) {
    _instruction._sop = sop;
    return *this;
  }

  Instruction opcode(uint8_t opcode) {
    _instruction._opcode_mode.opcode = opcode;
    return *this;
  }

  Instruction mode(uint8_t mod_reg) {
    _instruction._opcode_mode.mode = mod_reg;
    return *this;
  }

  Instruction offset(int16_t offset) {
    _instruction._offset = offset;
    return *this;
  }

  Instruction data(uint16_t data) {
    _instruction._imm.data = data;
    return *this;
  }

  Instruction port(uint8_t port) {
    _instruction._imm.lo = port;
    return *this;
  }

  operator sop_t() const { return _instruction._sop; }

  uint8_t sop() const { return _instruction._sop; }

  template <typename T> T opcode_to() const {
    return *(T *)&_instruction._opcode_mode.opcode;
  }

  template <typename T> T mode_to() const {
    return *(T *)&_instruction._opcode_mode.mode;
  }

  uint16_t offset() const { return _instruction._offset; }

  template <typename T> T data() const { return (T)_instruction._imm.data; }

  uint8_t port() const { return _instruction._imm.lo; }

  friend std::ostream &operator<<(std::ostream &ostream,
                                  const Instruction &instruction) {
    ostream << " "
            << fmt::format("sop=0x{0:x}, opcode=0x{1:x}(0b{1:b}), mode="
                           "0x{2:x}(0b{2:b}), offset=0x{3:x}, data=0x{4:x}",
                           instruction._instruction._sop,
                           instruction._instruction._opcode_mode.opcode,
                           instruction._instruction._opcode_mode.mode,
                           instruction._instruction._offset,
                           instruction._instruction._imm.data)
            << " ";
    return ostream;
  }
};

template <typename T, typename P> class InstructionTemplate {
  Instruction _instruction;

public:
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

// TODO fix pointer copy and assignments
struct Bytes {
  uint8_t *_bytes;
  uint16_t _size;
  Bytes() : _bytes(nullptr), _size(0) {}

  explicit Bytes(uint8_t *bytes, uint16_t size) : _bytes(bytes), _size(size) {}

  Bytes(uint8_t const *bytes, uint16_t size)
      : _bytes(const_cast<uint8_t *>(bytes)), _size(size) {} // NOLINT

  Bytes(const Bytes &bytes) : _bytes(bytes._bytes), _size(bytes._size) {}

  ~Bytes() {
    // delete _bytes;
    _bytes = nullptr;
  }

  Bytes &operator=(Bytes &&rhs) noexcept {
    std::swap(_size, rhs._size);
    std::swap(_bytes, rhs._bytes);
    return *this;
  }

  operator uint16_t() const {
    assert(_size == 2);
    return (uint16_t)((_bytes[1] << 8) | _bytes[0]); // NOLINT
  }

  operator uint8_t() const {
    assert(_size == 1);
    return _bytes[0]; // NOLINT
  }
};

struct BUS {
  virtual ~BUS() = default;
  virtual uint16_t write(Address const *, const Bytes &) = 0;
  virtual Bytes read(Address const *, uint16_t size) = 0;
};

class ValueIO final : public IO {
protected:
  u16_t _value;

public:
  explicit ValueIO() = default;

  ValueIO(const ValueIO &rhs) : _value(rhs._value) {}

  ValueIO &operator=(const uint16_t &val) {
    _value.word = val;
    return *this;
  }

  ValueIO &operator=(const uint8_t &val) {
    _value.lo = val;
    return *this;
  }

  void write_hi(const uint8_t val) override { _value.hi = val; }

  void write_lo(const uint8_t val) override { _value.lo = val; }

  void write(const uint8_t val) override { _value.lo = val; }

  void write(const uint16_t val) override { _value.word = val; }

  uint16_t read() const override { return _value.word; }

  uint8_t read_byte() const override { return _value.lo; }

  uint8_t read_hi() const override { return _value.hi; }

  uint8_t read_lo() const override { return _value.lo; }
};

class BUSIO final : public IO {
private:
  BUS *_bus;
  Address _address;

public:
  explicit BUSIO(BUS *bus) : _bus(bus) {}

  BUSIO(const BUSIO &rhs) : _bus(rhs._bus), _address(rhs._address) {}

  ~BUSIO() override = default;

  BUS *bus() const { return _bus; }

  Address address() const { return _address; }

  BUSIO set_address(const Address &address) {
    _address = address;
    return *this;
  }

  void write_hi(const uint8_t val) override {
    uint16_t _word = make_word(val, 0);
    // NOLINTNEXTLINE
    Bytes bytes(reinterpret_cast<uint8_t *>(&_word), sizeof(uint16_t));
    _bus->write(&_address, bytes);
  }

  void write_lo(const uint8_t val) override {
    Bytes bytes(const_cast<uint8_t *>(&val), sizeof(uint8_t)); // NOLINT
    _bus->write(&_address, bytes);
  }

  void write(const uint16_t val) override {
    // NOLINTNEXTLINE
    Bytes bytes(reinterpret_cast<const uint8_t *>(&val), sizeof(uint16_t));
    _bus->write(&_address, bytes);
  }

  void write(const uint8_t val) override {
    Bytes bytes(&val, sizeof(uint8_t));
    _bus->write(&_address, bytes);
  }

  uint16_t read() const override {
    Bytes bytes = _bus->read(&_address, sizeof(uint16_t));
    uint16_t word = 0;
    std::memcpy(&word, bytes._bytes, bytes._size);
    return word;
  }

  uint8_t read_byte() const override {
    Bytes bytes = _bus->read(&_address, sizeof(uint8_t));
    uint8_t byte = 0;
    std::memcpy(&byte, bytes._bytes, bytes._size);
    return byte;
  }

  uint8_t read_hi() const override {
    Bytes bytes = _bus->read(&_address, sizeof(uint16_t));
    uint16_t word = 0;
    std::memcpy(&word, bytes._bytes, sizeof(uint16_t));
    return ((word >> 8) & 0x107);
  }

  uint8_t read_lo() const override {
    Bytes bytes = _bus->read(&_address, sizeof(uint8_t));
    uint16_t byte = 0;
    std::memcpy(&byte, bytes._bytes, sizeof(uint8_t));
    return byte;
  }

  friend std::ostream &operator<<(std::ostream &ostream, const BUSIO &t) {
    ostream << fmt::format("BUSIO_ptr=0x{:x}", (long)&t) << ", "
            << fmt::format("physical_address=0x{:x}", (uint32_t)t._address);
    return ostream;
  }
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

struct Decoder {
  Decoder(bus_ptr_t bus = nullptr, registers_ptr_t registers = nullptr)
      : _bus(bus), _registers(registers) {}

  virtual ~Decoder() = default;
  virtual std::pair<IO *, IO *> decode(const Instruction &instruction) {
    return {source(instruction), destination(instruction)};
  };

protected:
  virtual IO *source(const Instruction &) = 0;
  virtual IO *destination(const Instruction &) = 0;

  bus_ptr_t _bus;
  registers_ptr_t _registers;
};

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

enum CJmpOpTypes {
  noj,
  je,
  jne,
  js,
  jns,
  jo,
  jno,
  jc,
  jb,
  jnae,
  jnc,
  jae,
  jnb,
  jp,
  jpe,
  jnp,
  jpo,
  jbe,
  jna,
  jnbe,
  ja,
  jle,
  jng,
  jnle,
  jg,
  jl,
  jnge,
  jnl,
  jge,
};

// NOLINTNEXTLINE
static const std::string _CJmpOpTypes[] = {
    "noj", "je",  "jne", "js",   "jns", "jo",  "jno",  "jc",  "jb",  "jnae",
    "jnc", "jae", "jnb", "jp",   "jpe", "jnp", "jpo",  "jbe", "jna", "jnbe",
    "ja",  "jle", "jng", "jnle", "jg",  "jl",  "jnge", "jnl", "jge",
};

struct OpType {
  virtual ~OpType() = default;
  struct Params {
    OpTypes _op_type;
    CJmpOpTypes _jmp_type;
    IO *_source, *_destination;
    Flags *_flags;

    Params(OpTypes op_type, IO *source, IO *destination)
        : _op_type(op_type), _jmp_type(noj), _source(source),
          _destination(destination), _flags(nullptr) {}

    Params(OpTypes op_type, IO *source, IO *destination, Flags *flags)
        : _op_type(op_type), _jmp_type(noj), _source(source),
          _destination(destination), _flags(flags) {}

    Params(OpTypes op_type, IO *source, IO *destination, Flags *flags,
           CJmpOpTypes jmp_type)
        : _op_type(op_type), _jmp_type(jmp_type), _source(source),
          _destination(destination), _flags(flags) {}

    Params &operator=(const Params &rhs) {
      _op_type = rhs._op_type;
      _jmp_type = rhs._jmp_type;
      _source = rhs._source;
      _destination = rhs._destination;
      _flags = rhs._flags;
      return *this;
    }
  };

  virtual void execute(const OpType::Params &params) const = 0;
};

struct OpTypeSelector {
  virtual ~OpTypeSelector() = default;
  virtual OpTypes op_type(const Instruction &instruction) const = 0;
};

struct MicroOp {

  struct ExecuteStrategy {
    virtual ~ExecuteStrategy() = default;
    ExecuteStrategy(Decoder *const decoder, MicroOp *const micro_op)
        : _decoder(decoder), _micro_op(micro_op) {}

    virtual void execute(const Instruction &instruction) const = 0;

    Decoder *_decoder;
    MicroOp *_micro_op;
  };

  struct Params {
    BUS *bus;
    Registers *registers;
    Params(BUS *bus = nullptr, Registers *registers = nullptr)
        : bus(bus), registers(registers) {}
  };

  struct Key {
    uint8_t _opcode;
    uint8_t _mask;

    Key(uint8_t opcode = 0x0, uint8_t mask = 0xFF)
        : _opcode(opcode), _mask(mask) {}

    Key(const Key &rhs) : _opcode(rhs._opcode), _mask(rhs._mask) {}

    bool operator<(const Key &rhs) const { return _opcode < (rhs & _mask); }

    uint8_t operator&(uint8_t mask) const { return _opcode & mask; }

    bool operator==(const Key &rhs) const { return _opcode == (rhs & _mask); }

    friend std::ostream &operator<<(std::ostream &ostream, const Key &key) {
      ostream << fmt::format("opcode=0x{:x}", key._opcode);
      return ostream;
    }
  };

  MicroOp(bus_ptr_t bus = nullptr, registers_ptr_t registers = nullptr)
      : _bus(bus), _registers(registers) {}

  MicroOp(const MicroOp &rhs) : _bus(rhs._bus), _registers(rhs._registers) {}

  virtual ~MicroOp() = default;

  virtual void execute(const Instruction &) = 0;

  virtual Instruction before_decode(const Instruction &instruction) {
    return instruction;
  }

  virtual void before_execute(UNUSED_PARAM const Instruction &instruction) {}

  virtual void after_execute(UNUSED_PARAM const Instruction &instruction) {}

  bus_ptr_t _bus;
  registers_ptr_t _registers;

protected:
  template <class OpTypeSelectorT, class OperatorT>
  struct ExecuteStrategy2 final : ExecuteStrategy {
    ExecuteStrategy2(Decoder *const decoder, MicroOp *const micro_op)
        : ExecuteStrategy(decoder, micro_op) {}

    void execute(const Instruction &instruction) const override {
      auto op_selector = OpTypeSelectorT();
      auto _instruction = _micro_op->before_decode(instruction);
      auto src_dest = _decoder->decode(_instruction);
      _micro_op->before_execute(_instruction);
      auto uop_operator =
          OperatorT(src_dest.first, src_dest.second, &op_selector);
      uop_operator.execute(_instruction);
      _micro_op->after_execute(_instruction);
    }
  };

  template <class OpTypeSelectorT, class OperatorT, class OpTypeT>
  struct ExecuteStrategy21 final : ExecuteStrategy {
    ExecuteStrategy21(Decoder *const decoder, MicroOp *const micro_op)
        : ExecuteStrategy(decoder, micro_op) {}

    void execute(const Instruction &instruction) const override {
      auto op_selector = OpTypeSelectorT();
      auto op_type = OpTypeT();
      auto _instruction = _micro_op->before_decode(instruction);
      auto src_dest = _decoder->decode(_instruction);
      auto *registers = _micro_op->_registers;
      _micro_op->before_execute(_instruction);
      auto uop_operator = OperatorT(src_dest.first, src_dest.second,
                                    &op_selector, &op_type, &registers->FLAGS);
      uop_operator.execute(_instruction);
      _micro_op->after_execute(_instruction);
    }
  };

  template <class OpTypeSelectorT, class OperatorT, class OpTypeT>
  struct ExecuteStrategy3 final : ExecuteStrategy {
    ExecuteStrategy3(Decoder *const decoder, MicroOp *const micro_op)
        : ExecuteStrategy(decoder, micro_op) {}

    void execute(const Instruction &instruction) const override {
      auto op_selector = OpTypeSelectorT();
      auto op_type = OpTypeT();
      auto _instruction = _micro_op->before_decode(instruction);
      auto src_dest = _decoder->decode(_instruction);
      auto *registers = _micro_op->_registers;
      _micro_op->before_execute(_instruction);
      auto uop_operator = OperatorT(src_dest.first, src_dest.second,
                                    &op_selector, &op_type, &registers->FLAGS);
      uop_operator.execute(_instruction);
      _micro_op->after_execute(_instruction);
    }
  };

  struct Executor final {
    Executor(ExecuteStrategy *const execute_strategy)
        : _execute_strategy(execute_strategy) {}

    void execute(const Instruction &instruction) const {
      _execute_strategy->execute(instruction);
    }

    ExecuteStrategy *_execute_strategy;
  };
};

#define MICRO_OP_INSTRUCTION(cls)                                              \
  static std::shared_ptr<MicroOp> create(const MicroOp::Params &params) {      \
    PLOGD << #cls << "::create";                                               \
    return std::make_shared<cls>(params.bus, params.registers);                \
  }

#define MICRO_OP_INSTRUCTION_D(cls, execute_strategy_cls, decoder_cls)         \
  static std::shared_ptr<MicroOp> create(const MicroOp::Params &params) {      \
    PLOGD << #cls << "::create";                                               \
    return std::make_shared<cls>(params.bus, params.registers);                \
  }                                                                            \
  void execute(const Instruction &instruction) override {                      \
    auto decoder = decoder_cls(_bus, _registers);                              \
    auto _strategy = execute_strategy_cls(&decoder, (MicroOp *)this);          \
    Executor(&_strategy).execute(instruction);                                 \
  }

#define MICRO_OP_INSTRUCTION_DCR(cls, op_type_selector_cls, operator_type_cls, \
                                 decoder_cls)                                  \
  static std::shared_ptr<MicroOp> create(const MicroOp::Params &params) {      \
    PLOGD << #cls << "::create";                                               \
    return std::make_shared<cls>(params.bus, params.registers);                \
  }                                                                            \
  void execute(const Instruction &instruction) override {                      \
    auto decoder = decoder_cls(_bus, _registers);                              \
    auto _strategy =                                                           \
        ExecuteStrategy2<op_type_selector_cls, operator_type_cls>(             \
            &decoder, (MicroOp *)this);                                        \
    Executor(&_strategy).execute(instruction);                                 \
  }

#define MICRO_OP_INSTRUCTION_DCRE(cls, op_type_selector_cls,                   \
                                  operator_type_cls, op_type_cls, decoder_cls) \
  static std::shared_ptr<MicroOp> create(const MicroOp::Params &params) {      \
    PLOGD << #cls << "::create";                                               \
    return std::make_shared<cls>(params.bus, params.registers);                \
  }                                                                            \
  void execute(const Instruction &instruction) override {                      \
    auto decoder = decoder_cls(_bus, _registers);                              \
    auto _strategy = ExecuteStrategy3<op_type_selector_cls, operator_type_cls, \
                                      op_type_cls>(&decoder, (MicroOp *)this); \
    Executor(&_strategy).execute(instruction);                                 \
  }

#endif /* TYPES_H_ */
