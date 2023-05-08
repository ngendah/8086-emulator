#ifndef _LDS_TESTS_H_
#define _LDS_TESTS_H_

#include "lds.h"
#include "logger.h"
#include "ram.h"
#include "gtest/gtest.h"

struct LDSTest final : LDS {
  LDSTest(bus_ptr_t bus, registers_ptr_t registers,
          std::function<void(LDS *const, uint8_t)> test_point)
      : LDS(bus, registers, test_point) {}

  Registers *get() const { return _registers; }
};

TEST(LDSTests, test_execute_1) {
  auto ram = RAM(125);
  auto address = Address((uint16_t)0x42);
  auto val = (uint8_t)55;
  auto bytes = Bytes(&val, 1);
  ram.write(&address, bytes);
  val = 0x97;
  bytes = Bytes(&val, 1);
  address = Address((uint16_t)0x44);
  ram.write(&address, bytes);
  auto registers = Registers();
  auto io = LDSTest(&ram, &registers, [](LDS *const les, uint8_t point) {
    auto registers = ((LDSTest *)les)->get();
    if (point == 0) {
      EXPECT_EQ((uint16_t)registers->BX, 55);
    }
  });
  // LDS BX, 0x42[SI]
  auto instruction = Instruction(0xff, 0xC55C, 0x42);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.DS, val);
}

#endif // _LDS_TESTS_H_