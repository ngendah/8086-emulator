#include "bus_io.h"

#include "bus.h"
#include "bytes.h"
#include "device_io.h"

AddressLatch::AddressLatch(BUS *bus) : _memory(bus->memory()) {}

AddressLatch::AddressLatch(bus_ptr_t bus, const Address &address)
    : _memory(bus->memory()), _address(address){};

AddressLatch::AddressLatch(const AddressLatch &rhs)
    : _memory(rhs._memory), _address(rhs._address) {}

Address AddressLatch::address() const { return _address; }

AddressLatch AddressLatch::set_address(const Address &address) {
  _address = address;
  return *this;
}

void AddressLatch::write_hi(const uint8_t val) {
  uint16_t _word = make_word(val, 0);
  // NOLINTNEXTLINE
  Bytes bytes(cast_ui8p(&_word), sizeof_ui16);
  _memory->write(&_address, bytes);
}

void AddressLatch::write_lo(const uint8_t val) {
  Bytes bytes(cast_ui8p(&val), sizeof_ui8); // NOLINT
  _memory->write(&_address, bytes);
}

void AddressLatch::write(const uint16_t val) {
  // NOLINTNEXTLINE
  Bytes bytes(cast_ui8p(&val), sizeof_ui16);
  _memory->write(&_address, bytes);
}

void AddressLatch::write(const uint8_t val) {
  Bytes bytes(cast_ui8p(&val), sizeof_ui8);
  _memory->write(&_address, bytes);
}

uint16_t AddressLatch::read() const {
  Bytes bytes = _memory->read(&_address, sizeof_ui16);
  uint16_t word = *cast_ui16p(bytes._bytes);
  return word;
}

uint8_t AddressLatch::read_byte() const {
  Bytes bytes = _memory->read(&_address, sizeof_ui8);
  uint8_t byte = *cast_ui8p(bytes._bytes);
  return byte;
}

uint8_t AddressLatch::read_hi() const {
  Bytes bytes = _memory->read(&_address, sizeof_ui16);
  uint16_t word = *cast_ui16p(bytes._bytes);
  return ((word >> 8) & 0xff);
}

uint8_t AddressLatch::read_lo() const {
  Bytes bytes = _memory->read(&_address, sizeof_ui8);
  return *cast_ui8p(bytes._bytes);
}

std::ostream &operator<<(std::ostream &ostream, const AddressLatch &t) {
  ostream << fmt::format("AddressLatch_ptr=0x{:x}", (long)&t) << ", "
          << fmt::format("physical_address=0x{:x}", cast_ui32(t._address));
  return ostream;
}

PortIO::PortIO(const PortIO &rhs) : _device(rhs._device) {}

void PortIO::device(DeviceIO *device) { _device = device; }

void PortIO::write_hi(const uint8_t val) {
  uint16_t _word = make_word(val, 0);
  // NOLINTNEXTLINE
  Bytes bytes(cast_ui8p(&_word), sizeof_ui16);
  auto _address = Address();
  _device->write(&_address, bytes);
}

void PortIO::write_lo(const uint8_t val) {
  Bytes bytes(cast_ui8p(&val), sizeof_ui8); // NOLINT
  auto _address = Address();
  _device->write(&_address, bytes);
}

void PortIO::write(const uint16_t val) {
  // NOLINTNEXTLINE
  Bytes bytes(cast_ui8p(&val), sizeof_ui16);
  auto _address = Address();
  _device->write(&_address, bytes);
}

void PortIO::write(const uint8_t val) {
  Bytes bytes(cast_ui8p(&val), sizeof_ui8);
  auto _address = Address();
  _device->write(&_address, bytes);
}

uint16_t PortIO::read() const {
  auto _address = Address();
  Bytes bytes = _device->read(&_address, sizeof_ui16);
  uint16_t word = *cast_ui16p(bytes._bytes);
  return word;
}

uint8_t PortIO::read_byte() const {
  auto _address = Address();
  Bytes bytes = _device->read(&_address, sizeof_ui8);
  uint8_t byte = *cast_ui8p(bytes._bytes);
  return byte;
}

uint8_t PortIO::read_hi() const {
  auto _address = Address();
  Bytes bytes = _device->read(&_address, sizeof_ui16);
  uint16_t word = *cast_ui16p(bytes._bytes);
  return ((word >> 8) & 0xff);
}

uint8_t PortIO::read_lo() const {
  auto _address = Address();
  Bytes bytes = _device->read(&_address, sizeof_ui8);
  return *cast_ui8p(bytes._bytes);
}
