#ifndef _LAHF_TESTS_H_
#define _LAHF_TESTS_H_

#include "lahf.h"
#include "logger.h"
#include "ram.h"

#include "gtest/gtest.h"

TEST(LAHFTests, test_execute) {
  auto ram = RAM(128);
  auto registers = Registers();
  registers.FLAGS.write((uint16_t)0x12F6);
  auto io = LAHF(&ram, &registers);
  io.execute(Instruction(0xff, 0x9F00));
  EXPECT_EQ(registers.AX.read_hi(), 0xF6);
}

#endif // _LAHF_TESTS_H_