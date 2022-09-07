#ifndef _SAHF_TESTS_H_
#define _SAHF_TESTS_H_

#include "logger.h"
#include "ram.h"
#include "sahf.h"
#include "gtest/gtest.h"

TEST(SAHFTests, test_execute) {
  auto ram = RAM(128);
  auto registers = Registers();
  registers.AX.write((uint16_t)0x12F6);
  auto io = SAHF(&ram, &registers);
  io.execute(Instruction(0xff, 0x9E00));
  EXPECT_EQ(registers.FLAGS.read_lo(), 0x12);
}

#endif // _SAHF_TESTS_H_