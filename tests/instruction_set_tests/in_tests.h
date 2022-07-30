#ifndef _IN_TESTS_H_
#define _IN_TESTS_H_

#include "in.h"
#include "logger.h"
#include "ram.h"
#include "gtest/gtest.h"

TEST(INPortTests, test_execute) {
  auto ram = RAM(128);
  auto registers = Registers();
  auto address = Address((uint8_t)0x53);
  uint16_t val = 78;
  auto bytes = Bytes((uint8_t *)&val, sizeof(val));
  ram.write(&address, bytes);
  auto instruction = Instruction(0xff, 0xE400, 0x0, 0x0, (uint8_t)address);
  auto io = INPort(&ram, &registers);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.AX, val);
}

#endif // _IN_TESTS_H_
