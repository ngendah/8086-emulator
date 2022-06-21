#ifndef TYPES_H_
#define TYPES_H_

#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <fmt/core.h>
#include <iostream>

#define len(x) sizeof(x) * CHAR_BIT

typedef struct {
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
} flags_t;

typedef uint8_t sop_t; // segment override prefix

static uint16_t make_word(const uint8_t hi, const uint8_t lo) {
  return ((uint16_t)((hi << 8) | (lo & 0x00FF)));
}
class Instruction final {
private:
  uint8_t _sop;
  uint8_t _opcode;
  uint8_t _mode;
  uint16_t _offset;
  uint16_t _data;

public:
  Instruction() : _sop(0xff), _opcode(0), _mode(0), _offset(0), _data(0) {}

  Instruction(uint8_t sop, uint16_t opcode_mode, uint16_t offset = 0,
              uint16_t data = 0)
      : _sop(sop), _opcode((uint8_t)(opcode_mode >> 8)),
        _mode((uint8_t)opcode_mode), _offset(offset), _data(data) {}

  Instruction(const Instruction &rhs)
      : _sop(rhs._sop), _opcode(rhs._opcode), _mode(rhs._mode),
        _offset(rhs._offset), _data(rhs._data) {}

  Instruction(Instruction &rhs)
      : _sop(rhs._sop), _opcode(rhs._opcode), _mode(rhs._mode),
        _offset(rhs._offset), _data(rhs._data) {}

  ~Instruction() = default;

  Instruction sop(uint8_t sop) {
    _sop = sop;
    return *this;
  }

  Instruction opcode(uint8_t opcode) {
    _opcode = opcode;
    return *this;
  }

  Instruction mode(uint8_t mod_reg) {
    _mode = mod_reg;
    return *this;
  }

  Instruction offset(int16_t offset) {
    _offset = offset;
    return *this;
  }

  Instruction data(uint16_t data) {
    _data = data;
    return *this;
  }

  operator sop_t() const { return _sop; }

  uint16_t sop() const { return _sop; }

  template <typename T> T opcode_to() const { return *(T *)&_opcode; }

  template <typename T> T mode_to() const { return *(T *)&_mode; }

  uint16_t offset() const { return _offset; }

  template <typename T> T data() const { return (T)_data; }

  friend std::ostream &operator<<(std::ostream &os,
                                  const Instruction &instruction) {
    os << " "
       << fmt::format("sop=0x{0:x}, opcode=0x{1:x}(0b{1:b}), mode="
                      "0x{2:x}(0b{2:b}), offset=0x{3:x}, data=0x{4:x}",
                      instruction._sop, instruction._opcode, instruction._mode,
                      instruction._offset, instruction._data)
       << " ";
    return os;
  }
};

template <typename T, typename P> class InstructionTemplate {
  Instruction _instruction;

public:
  InstructionTemplate() = default;

  InstructionTemplate(const Instruction &val) : _instruction(val) {}

  InstructionTemplate(const InstructionTemplate<T, P> &rhs)
      : _instruction(rhs._instruction) {}

  T opcode() const { return _instruction.opcode_to<T>(); }

  P mode() const { return _instruction.mode_to<P>(); }

  friend std::ostream &
  operator<<(std::ostream &os,
             const InstructionTemplate<T, P> &instruction_template) {
    os << instruction_template._instruction;
    return os;
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
  Address operator=(const Address &rhs) {
    _address = rhs._address;
    return *this;
  }
  operator uint8_t() const { return (uint8_t)_address & 0x000000ff; }
  operator uint16_t() const { return (uint16_t)_address & 0x0000ffff; }
  operator uint32_t() const { return (uint32_t)_address & 0x000fffff; }
};

class InstructionCode final {
  uint8_t _code;
  uint8_t _mask;

public:
  InstructionCode(uint8_t code = 0x0, uint8_t mask = 0x0)
      : _code(code), _mask(mask) {}

  virtual ~InstructionCode() = default;

  virtual bool operator==(const uint8_t &code) {
    return this->_code == (this->_mask & code);
  }
};

struct Clock {
  virtual ~Clock();
};

struct Decoder {
  virtual ~Decoder() = default;
  virtual void decodeAndExecute();
  virtual uint16_t fetch(uint16_t);
};

struct Bytes {
  uint8_t *_bytes;
  uint16_t _size;
  Bytes() : _bytes(nullptr), _size(0) {}

  explicit Bytes(uint8_t *bytes, uint16_t size) : _bytes(nullptr), _size(size) {
    _bytes = bytes;
    _size = size;
  }

  Bytes(const Bytes &bytes) : _bytes(nullptr), _size(0) {
    _bytes = bytes._bytes;
    _size = bytes._size;
  }

  ~Bytes() { _bytes = nullptr; }

  operator uint16_t() const {
    assert(_size == 2);
    return (uint16_t)((_bytes[1] << 8) | _bytes[0]);
  }

  operator uint8_t() const {
    assert(_size == 1);
    return _bytes[0];
  }
};

struct BUS {
  virtual ~BUS() = default;
  virtual uint16_t write(Address *, const Bytes &) = 0;
  virtual Bytes read(Address *, uint16_t size) = 0;
};

struct IO {
  virtual ~IO() = default;

  virtual void write_hi(const uint8_t val) = 0;

  virtual void write_lo(const uint8_t val) = 0;

  virtual void write(const uint8_t val) = 0;

  virtual void write(const uint16_t val) = 0;

  virtual uint16_t read() const = 0;

  virtual uint8_t read_byte() const = 0;

  virtual uint8_t read_hi() const = 0;

  virtual uint8_t read_lo() const = 0;
};

class ValueIO final : public IO {
  enum ValueType {
    word,
    byte,
  };

  struct Value {
    ValueType _size;
    uint16_t _value;

    Value() : _size(word), _value(0) {}

    Value(const Value &rhs) : _size(rhs._size), _value(rhs._value) {}

    ~Value() = default;

    Value operator=(const uint8_t &value) {
      _size = byte;
      _value = value;
      return *this;
    }

    Value operator=(const uint16_t &value) {
      _size = word;
      _value = value;
      return *this;
    }

    operator uint16_t() const { return _value; }

    operator uint8_t() const { return (uint8_t)_value; }
  };

  Value _value;

public:
  explicit ValueIO() = default;

  ValueIO(const ValueIO &rhs) : _value(rhs._value) {}

  ValueIO operator=(const uint16_t &val) {
    _value = val;
    return *this;
  }

  ValueIO operator=(const uint8_t &val) {
    _value = val;
    return *this;
  }

  void write_hi(const uint8_t val) override { _value = make_word(val, 0); }

  void write_lo(const uint8_t val) override { _value = make_word(0, val); }

  void write(const uint8_t val) override { _value = val; }

  void write(const uint16_t val) override { _value = val; }

  uint16_t read() const override { return _value; }

  uint8_t read_byte() const override { return (uint8_t)_value; }

  uint8_t read_hi() const override {
    return (uint8_t)(((uint16_t)_value) >> 8);
  }

  uint8_t read_lo() const override { return (uint8_t)_value; }
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
    Bytes bytes((uint8_t *)&_word, sizeof(uint16_t));
    _bus->write(&_address, bytes);
  }

  void write_lo(const uint8_t val) override {
    Bytes bytes((uint8_t *)&val, sizeof(uint8_t));
    _bus->write(&_address, bytes);
  }

  void write(const uint16_t val) override {
    Bytes bytes((uint8_t *)&val, sizeof(uint16_t));
    _bus->write(&_address, bytes);
  }

  void write(const uint8_t val) override {
    Bytes bytes((uint8_t *)&val, sizeof(uint8_t));
    _bus->write(&_address, bytes);
  }

  uint16_t read() const override {
    Bytes bytes = _bus->read((Address *)&_address, sizeof(uint16_t));
    uint16_t word = 0;
    std::memcpy(&word, bytes._bytes, bytes._size);
    return word;
  }

  uint8_t read_byte() const override {
    Bytes bytes = _bus->read((Address *)&_address, sizeof(uint8_t));
    uint8_t byte = 0;
    std::memcpy(&byte, bytes._bytes, bytes._size);
    return byte;
  }

  uint8_t read_hi() const override {
    Bytes bytes = _bus->read((Address *)&_address, sizeof(uint16_t));
    uint16_t word = 0;
    std::memcpy(&word, bytes._bytes, sizeof(uint16_t));
    return ((word >> 8) & 0x107);
  }

  uint8_t read_lo() const override {
    Bytes bytes = _bus->read((Address *)&_address, sizeof(uint8_t));
    uint16_t byte = 0;
    std::memcpy(&byte, bytes._bytes, sizeof(uint8_t));
    return byte;
  }

  friend std::ostream &operator<<(std::ostream &os, const BUSIO &t) {
    os << "BUSIO{"
       << fmt::format("_bus=0x{0:x}, _address=0x{1:x}", (long)t._bus,
                      (uint32_t)t._address)
       << "}";
    return os;
  }
};

class Register : public IO {
  uint16_t _register;

public:
  Register() : _register(0) {}

  Register(const Register &rhs) : _register(rhs._register) {}

  Register(const uint16_t val) : _register(val) {}

  ~Register() override = default;

  void write_hi(const uint8_t val) override { _register = make_word(val, 0); }

  void write_lo(const uint8_t val) override { _register = make_word(0, val); }

  void write(const uint16_t val) override { _register = val; }

  void write(const uint8_t val) override { _register = val; }

  uint16_t read() const override { return _register; }

  uint8_t read_hi() const override {
    return (uint8_t)((_register >> 8) & 0xff);
  }

  uint8_t read_lo() const override { return (uint8_t)(_register & 0xff); }

  uint8_t read_byte() const override { return (uint8_t)_register; }

  operator uint16_t() const { return _register; }

  Register operator+=(uint16_t offset) {
    _register += offset;
    return *this;
  }
};

struct Segment final : public Register {
  Segment() = default;

  Segment(const Segment &rhs) : Register(rhs) {}

  Segment(const uint16_t val) : Register(val) {}

  ~Segment() override = default;

  Address address(const Register &reg, uint16_t offset = 0x0) {
    uint16_t eff_addr = offset + (uint16_t)reg;
    return Address((uint32_t)(((uint32_t) * this) * 0x10 + eff_addr));
  }

  Address address(const uint16_t &eff_addr) {
    return Address((uint32_t)(((uint32_t) * this) * 0x10 + eff_addr));
  }
};

struct Registers final {
  Register AX, BX, CX, DX, SP, BP, SI, DI, IP;
  Segment CS, DS, SS, ES;
  flags_t FLAGS;
};

#endif /* RAM_H_ */
