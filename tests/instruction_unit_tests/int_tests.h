#ifndef _INT_TESTS_H_
#define _INT_TESTS_H_

#include "int.h"
#include "logger.h"
#include "ram.h"

#include "gtest/gtest.h"
#include <array>

TEST(INTTests, test_before_execute) {
  std::array<uint8_t, 125> buffer{};
  auto ram = RAM(&buffer.at(0), 125);
  auto registers = Registers();
  registers.SP = 0x25;
  registers.FLAGS.set(0xffff);
  registers.CS = 0x35;
  registers.IP = 0x55;
  auto _int = INT(&ram, &registers);
  _int.before_execute(Instruction());
  // at this point the SP points to the next address to write to
  // since its full descending we add a word to get the
  // previous memory that was written to.
  auto physical_addresser = PhysicalAddresser(&registers);
  auto address =
      physical_addresser.address(&registers.SS, registers.SP) + (uint16_t)2;
  auto value = ram.read_u16(&address);
  // IP was the last value to be pushed to the stack
  EXPECT_EQ((uint16_t)registers.IP, value);
}

#endif // _INT_TESTS_H_
