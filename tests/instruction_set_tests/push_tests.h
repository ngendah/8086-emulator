#ifndef _PUSH_TESTS_H_
#define _PUSH_TESTS_H_

#include "logger.h"
#include "push.h"
#include "ram.h"
#include "gtest/gtest.h"

TEST(PushRegisterTests, test_execute) {
  auto registers = Registers();
  registers.SS = (uint16_t)0x10;
  registers.SI += 0x37;
  auto ram = RAM(512);
  auto io = PushRegister(&registers, &ram);
  io.execute(Instruction(0xff, 0x5600));
  auto address = Address((uint16_t)0x100);
  auto bytes = ram.read(&address, sizeof(uint16_t));
  EXPECT_EQ((uint16_t)bytes, 0x37);
}

#endif // _PUSH_TESTS_H_