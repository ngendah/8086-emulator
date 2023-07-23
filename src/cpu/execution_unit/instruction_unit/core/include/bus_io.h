#ifndef _BUS_IO_H_
#define _BUS_IO_H_

#include "address.h"
#include "device_io.h"
#include "io.h"

#include <cstring>
#include <fmt/core.h>
#include <iostream>

struct AddressLatch final : IO {
  explicit AddressLatch(BUS *bus);
  AddressLatch(bus_ptr_t bus, const Address &address);
  AddressLatch(const AddressLatch &rhs);
  ~AddressLatch() override = default;

  Address address() const;
  AddressLatch set_address(const Address &);
  void write_hi(const uint8_t val) override;
  void write_lo(const uint8_t val) override;
  void write(const uint16_t val) override;
  void write(const uint8_t val) override;
  uint16_t read() const override;
  uint8_t read_byte() const override;
  uint8_t read_hi() const override;
  uint8_t read_lo() const override;

  friend std::ostream &operator<<(std::ostream &ostream, const AddressLatch &t);

private:
  DeviceIO *_memory;
  Address _address;
};

struct PortIO final : IO {
  PortIO() : _device(nullptr) {}
  PortIO(const PortIO &rhs);
  ~PortIO() override = default;

  void device(DeviceIO *);
  void write_hi(const uint8_t val) override;
  void write_lo(const uint8_t val) override;
  void write(const uint16_t val) override;
  void write(const uint8_t val) override;
  uint16_t read() const override;
  uint8_t read_byte() const override;
  uint8_t read_hi() const override;
  uint8_t read_lo() const override;

private:
  DeviceIO *_device;
};

#endif // _BUS_IO_H_
