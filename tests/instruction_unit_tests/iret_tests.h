#ifndef _IRET_TESTS_H_
#define _IRET_TESTS_H_

#include "int.h"
#include "iret.h"
#include "logger.h"
#include "ram.h"

#include "gtest/gtest.h"
#include <array>

TEST(IRETTests, test_execute) {
  std::array<uint8_t, 125> buffer{};
  auto ram = RAM(&buffer.at(0), 125);
  auto bus = AddressLatch(&ram);
  auto registers = Registers();
  registers.SP = 0x25;
  {
    registers.FLAGS.set(0xFFFF);
    registers.CS = 0x35FF;
    registers.IP = 0xEA55;
    auto intr = INT(&ram, &registers);
    intr.before_execute(Instruction());
  }
  // clear values
  registers.FLAGS.set(0);
  registers.CS = 0;
  registers.IP = 0;
  auto iret = IRET(&ram, &registers);
  iret.execute(Instruction());
  EXPECT_EQ(cast_ui16(registers.IP), 0xEA55);
  EXPECT_EQ(cast_ui16(registers.CS), 0x35FF);
  EXPECT_EQ(cast_ui16(registers.FLAGS), 0xFFFF);
}

#endif // _IRET_TESTS_H_
