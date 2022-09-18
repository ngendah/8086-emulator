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
  auto val = Extensions::Bytes((uint8_t)78);
  ram.write(&address, val);
  auto instruction = Instruction(0xff, 0xE400, 0x0, (uint8_t)address);
  auto io = INPort(&ram, &registers);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.AX, (uint16_t)val);
}

TEST(INDXTests, test_execute) {
  auto ram = RAM(12);
  auto registers = Registers();
  registers.DX.write((uint16_t)78);
  auto instruction = Instruction(0xff, 0xEC);
  auto io = INDX(&ram, &registers);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.AX, (uint16_t)78);
}

#endif // _IN_TESTS_H_
