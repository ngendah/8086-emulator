#include "device_io.h"

#include "types.h"

uint16_t DeviceIO::read_u16(Address const *address) {
  auto bytes = this->read(address, sizeof_ui16);
  return bytes;
}

uint8_t DeviceIO::read_u8(Address const *address) {
  auto bytes = this->read(address, sizeof_ui8);
  return bytes;
}
