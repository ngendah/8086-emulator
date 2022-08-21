#ifndef _INCR_TESTS_H_
#define _INCR_TESTS_H_

#include "incr.h"
#include "logger.h"
#include "ram.h"
#include "gtest/gtest.h"

TEST(INCRTests, test_execute) {
  auto ram = RAM(128);
  auto registers = Registers();
  auto io = INCRRegister(&ram, &registers);
  io.execute(Instruction(0xff, 0x4100));
  EXPECT_EQ((uint16_t)registers.CX, 0x1);
}

#endif // _INCR_TESTS_H_