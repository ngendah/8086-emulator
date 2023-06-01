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
  (void)_int;
}

#endif // _INT_TESTS_H_
