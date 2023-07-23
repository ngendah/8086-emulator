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
  uint8_t _buffer[125] = {};
  auto ram = RAM(_buffer, 125);
  auto bus = BUS::from_device(&ram);
  auto address = Address((uint16_t)0x42);
  auto val = (uint8_t)55;
  auto bytes = Bytes(&val, 1);
  ram.write(&address, bytes);
  val = 0x97;
  bytes = Bytes(&val, 1);
  address = Address((uint16_t)0x44);
  ram.write(&address, bytes);
  auto registers = Registers();
  auto io = LESTest(&bus, &registers, [](LES *const les, uint8_t point) {
    auto registers = ((LESTest *)les)->get();
    if (point == 0) {
      EXPECT_EQ((uint16_t)registers->BX, 55);
    }
  });
  // LES BX, 0x42[SI]
  auto instruction = Instruction(0xff, 0xC45C, 0x42);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.ES, val);
}

#endif // _LES_TESTS_H_
