#include "gtest/gtest.h"
#include "ram.h"

TEST(BufferTest, test_buffer_read_write) {
  auto buffer = Buffer(10);
  auto address = Address((uint8_t)5);
  uint16_t value = 0xAAE4;
  buffer.write(&address, Bytes((uint8_t*)&value, sizeof(uint16_t)));
  EXPECT_EQ(value, (uint16_t)buffer.read(&address, sizeof(uint16_t)));
}


void ram_tests(){}