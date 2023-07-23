//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _BUS_H_
#define _BUS_H_

#include <cstdint>
#include <map>
#include <memory>
#include <streambuf>

struct DeviceIO;
struct InterruptHandler;

struct Devices {
  typedef std::map<uint8_t, DeviceIO *> devices_t;

  explicit Devices() = default;
  Devices(const devices_t devices) : _devices(devices) {}
  Devices(const Devices &other) : _devices(other._devices) {}
  DeviceIO *device(uint8_t port) const;
  void attach(uint8_t port, DeviceIO *device);
  void detach(uint8_t port);
  void initialize(InterruptHandler *);

protected:
  devices_t _devices = {};
};

struct BUS {
  explicit BUS() = default;
  BUS(const Devices::devices_t &devices) : _devices(devices) {}
  BUS(const BUS &other) : _devices(other._devices) {}

  DeviceIO *memory() const {
    return io(0);
  } // TODO remove hardcoding of Memory device port
  DeviceIO *io(uint8_t port) const { return _devices.device(port); }
  Devices &devices() { return _devices; }

#ifdef _TESTS_H_
  static BUS from_device(DeviceIO *device) {
    return BUS(Devices::devices_t{{0, device}});
  }
#endif

protected:
  Devices _devices;
};

#endif // _BUS_H_
