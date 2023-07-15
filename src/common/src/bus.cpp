#include "bus.h"
#include "types.h"

uint16_t BUS::read_u16(Address const *address) {
  auto bytes = this->read(address, sizeof_ui16);
  return (uint16_t)bytes;
}

uint8_t BUS::read_u8(Address const *address) {
  auto bytes = this->read(address, sizeof_ui8);
  return (uint8_t)bytes;
}
