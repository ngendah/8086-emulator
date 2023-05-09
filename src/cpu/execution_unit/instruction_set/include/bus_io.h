
#ifndef _BUS_IO_H_
#define _BUS_IO_H_

#include "address.h"
#include "bytes.h"
#include "types.h"

struct BUSIO final : IO {
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
    Bytes bytes((uint8_t *)&val, sizeof(uint16_t));
    _bus->write(&_address, bytes);
  }

  void write(const uint8_t val) override {
    Bytes bytes((uint8_t *)&val, sizeof(uint8_t));
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

private:
  BUS *_bus;
  Address _address;
};

#endif // _BUS_IO_H_
