#include "bus.h"

#include "device.h"
#include "types.h"

DeviceIO *Devices::device(uint8_t port) const { return _devices.at(port); }

void Devices::attach(uint8_t port, DeviceIO *device) {
  _devices[port] = device;
}

void Devices::detach(uint8_t port) { _devices.erase(port); }

void Devices::initialize(InterruptHandler *handler) {
  for (auto device : _devices) {
    reinterpret_cast<Device *>(device.second)->initialize(handler);
  }
}
