#ifndef _XLAT_TESTS_H_
#define _XLAT_TESTS_H_

#include "logger.h"
#include "ram.h"
#include "xlat.h"

#include "gtest/gtest.h"

TEST(XLATTests, test_execute) {
  auto ram = RAM(256);
  auto bus = BUS::from_device(&ram);
  auto registers = Registers();
  registers.DS.write((u_int8_t)0x5);
  registers.AX.write((uint8_t)0x2);
  registers.BX.write((uint8_t)0x15);
  auto address =
      registers.DS.address(registers.AX.read() + registers.BX.read());
  uint16_t _val = 0x25;
  auto val = Bytes((uint8_t *)&_val, sizeof_ui16);
  ram.write(&address, val);
  auto instruction = Instruction(0xff, 0xD7);
  auto io = XLAT(&bus, &registers);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.AX, (uint16_t)val);
}

#endif // _XLAT_H_
