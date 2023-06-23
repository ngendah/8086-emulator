#include "bus.h"

uint16_t BUS::read_u16(Address const *address) {
  auto bytes = this->read(address, sizeof(uint16_t));
  return (uint16_t)bytes;
}

uint8_t BUS::read_u8(Address const *address) {
  auto bytes = this->read(address, sizeof(uint8_t));
  return (uint8_t)bytes;
}
