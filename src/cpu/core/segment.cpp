#include "address.h"
#include "types.h"

Address Segment::address(const Register &reg, uint16_t offset) {
  uint16_t eff_addr = offset + (uint16_t)reg;
  return Address((uint32_t)(((uint32_t) * this) * 0x10 + eff_addr));
}

Address Segment::address(uint16_t eff_addr) {
  return Address((uint32_t)(((uint32_t) * this) * 0x10 + eff_addr));
}
