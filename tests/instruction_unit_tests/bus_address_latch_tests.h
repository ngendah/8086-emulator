#ifndef _BUS_ADDRESS_LATCH_H_
#define _BUS_ADDRESS_LATCH_H_

#include "bus_io.h"
#include "ram.h"

#include "gtest/gtest.h"
#include <array>

TEST(AddressLatchTests, test_latch) {
  std::array<uint8_t, 125> _buffer{};
  RAM _ram(&_buffer.at(0), 125);
  {
    AddressLatch busio(&_ram, Address((uint16_t)45));
    busio.to_u16().write_hi(0xFD);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)45));
    EXPECT_EQ(busio.to_u16().read_hi(), 0xFD);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)45));
    busio.to_u16().write_lo(0xFD);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)45));
    EXPECT_EQ(busio.to_u16().read_lo(), 0xFD);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)25));
    busio.to_u16().write(0x4523);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)25));
    EXPECT_EQ(busio.to_u16().read(), 0x4523);
    EXPECT_EQ(busio.to_u16().read_hi(), 0x45);
    EXPECT_EQ(busio.to_u16().read_lo(), 0x23);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)10));
    busio.to_u8().write(0xAD);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)11));
    busio.to_u8().write(0xBA);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)10));
    EXPECT_EQ(busio.to_u16().read(), 0xBAAD);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)35));
    busio.to_u8().write(0x11);
  }
  {
    AddressLatch busio(&_ram, Address((uint16_t)35));
    EXPECT_EQ(busio.to_u8().read(), 0x11);
  }
}

#endif // _BUS_ADDRESS_LATCH_H_
