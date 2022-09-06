#ifndef _LES_TESTS_H_
#define _LES_TESTS_H_

#include "les.h"
#include "logger.h"
#include "ram.h"
#include "gtest/gtest.h"

struct LESTest final : LES {
  LESTest(bus_ptr_t bus, registers_ptr_t registers,
          std::function<void(LES *const, uint8_t)> test_point)
      : LES(bus, registers, test_point) {}

  Registers *get() const { return _registers; }
};

TEST(LESTests, test_execute_1) {
  auto ram = RAM(125);
  auto address = Address((uint16_t)0x42);
  auto val = (uint8_t)55;
  auto bytes = Bytes(&val, 1);
  ram.write(&address, bytes);
  val = 0xA;
  bytes = Bytes(&val, 1);
  address = Address((uint16_t)57);
  ram.write(&address, bytes);
  auto registers = Registers();
  auto io = LESTest(&ram, &registers, [](LES *const les, uint8_t point) {
    auto registers = ((LESTest *)les)->get();
    if (point == 0)
      EXPECT_EQ((uint16_t)registers->BX, 55);
    if (point == 1)
      EXPECT_EQ((uint16_t)registers->BX, 57);
    if (point == 2)
      EXPECT_EQ((uint16_t)registers->SI, 57);
  });
  // LES BX, 0x42[SI]
  auto instruction = Instruction(0xff, 0xC45C, 0x42);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.ES, val);
}

#endif // _LES_TESTS_H_