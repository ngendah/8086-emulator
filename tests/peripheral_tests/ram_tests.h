#include "gtest/gtest.h"
#include "ram.h"
#include <array>

TEST(StreamBufferTest, test_buffer_read_write) {
  std::array<uint8_t, 10>  _buffer{};
  auto buffer = stream_buffer(&_buffer.at(0), 10);
  auto address = Address((uint8_t)5);
  uint16_t wval = 0xAAE4;
  EXPECT_NE(buffer.pseekpos((uint16_t)address), -1);
  EXPECT_EQ(buffer.sputn((uint8_t*)&wval, sizeof(uint16_t)), 2);
  uint16_t rval=0;
  EXPECT_NE(buffer.gseekpos((uint16_t)address), -1);
  EXPECT_NE(buffer.sgetn((uint8_t*)&rval, sizeof(uint16_t)), -1);
  EXPECT_EQ(wval, rval);
}

TEST(StreamBufferTest, test_buffer_read_write_2) {
  std::array<uint8_t, 10>  _buffer{};
  auto buffer = stream_buffer(&_buffer.at(0), 10);
  auto address = Address((uint8_t)5);
  uint16_t wval = 0xAAE4;
  EXPECT_NE(buffer.seekpos((uint16_t)address, std::ios_base::in), -1);
  EXPECT_EQ(buffer.sputn((uint8_t*)&wval, sizeof(uint16_t)), 2);
  uint16_t rval=0;
  EXPECT_NE(buffer.seekpos((uint16_t)address, std::ios_base::out), -1);
  EXPECT_NE(buffer.sgetn((uint8_t*)&rval, sizeof(uint16_t)), -1);
  EXPECT_EQ(wval, rval);
}

void ram_tests(){}