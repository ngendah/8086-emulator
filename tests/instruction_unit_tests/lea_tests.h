#ifndef _LEA_TESTS_H_
#define _LEA_TESTS_H_

#include "lea.h"
#include "logger.h"
#include "ram.h"

#include "gtest/gtest.h"

TEST(LEATests, test_execute_1) {
  auto ram = RAM(125);
  auto registers = Registers();
  registers.SI = 0x15E0;
  auto lea = LEA(&ram, &registers);
  lea.execute(Instruction(0xff, 0x8D9C, 0x0053));
  EXPECT_EQ((uint16_t)registers.BX, 0x1633);
}

TEST(LEATests, test_execute_2) {
  auto ram = RAM(125);
  auto registers = Registers();
  registers.BP = 0x15E0;
  auto lea = LEA(&ram, &registers);
  lea.execute(Instruction(0xff, 0x8D76, 0x2B));
  EXPECT_EQ((uint16_t)registers.SI, 0x160B);
}

TEST(LEATests, test_execute_3) {
  auto ram = RAM(125);
  auto registers = Registers();
  registers.SI = 0x25;
  auto lea = LEA(&ram, &registers);
  lea.execute(Instruction(0xff, 0x8D1C));
  EXPECT_EQ((uint16_t)registers.BX, 0x25);
}

#endif // _LEA_TESTS_H_
