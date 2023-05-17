#ifndef _PUSH_TESTS_H_
#define _PUSH_TESTS_H_

#include "logger.h"
#include "push.h"
#include "ram.h"

#include "gtest/gtest.h"

TEST(PushRegisterTests, test_execute) {
  auto registers = Registers();
  registers.SS = 0x10;
  registers.SI = 0x37;
  auto ram = RAM(512);
  auto io = PushRegister(&ram, &registers, &stack_full_ascending);
  io.execute(Instruction(0xff, 0x5600));
  auto address = Address((uint16_t)0x100);
  auto bytes = ram.read(&address, sizeof(uint16_t));
  EXPECT_EQ((uint16_t)bytes, 0x37);
}

TEST(PushMemoryTests, test_execute) {
  auto registers = Registers();
  registers.SS = 0x10;
  auto ram = RAM(512);
  uint16_t val = 0x37;
  auto address = Address((uint16_t)0x0083);
  auto bytes = Bytes((uint8_t *)&val, sizeof(uint16_t));
  ram.write(&address, bytes);
  auto io = PushMemory(&ram, &registers, &stack_full_ascending);
  io.execute(Instruction(0xff, 0xFFB4, address));
  {
    auto address = Address((uint16_t)0x100);
    auto bytes = ram.read(&address, sizeof(uint16_t));
    EXPECT_EQ((uint16_t)bytes, 0x37);
  }
}

TEST(PushSegmentTests, test_execute) {
  auto registers = Registers();
  registers.SS = 0x10;
  registers.DS = 0x37;
  auto ram = RAM(512);
  auto io = PushSegment(&ram, &registers, &stack_full_ascending);
  io.execute(Instruction(0xff, 0x1E00));
  auto address = Address((uint16_t)0x100);
  auto bytes = ram.read(&address, sizeof(uint16_t));
  EXPECT_EQ((uint16_t)bytes, 0x37);
}

TEST(PushTests, test_execute) {
  auto ram = RAM(1024);
  auto registers = Registers();
  registers.SS = 0x10;
  registers.BP = 0x2;
  registers.SP = 0x2;
  {
    registers.BX = 0xE45B;
    auto io = PushRegister(&ram, &registers, &stack_full_ascending);
    io.execute(Instruction(0xff, 0x5300));
  }

  {
    uint16_t val = 0xA74E;
    auto address = Address((uint16_t)0x0083);
    auto bytes = Bytes((uint8_t *)&val, sizeof(uint16_t));
    ram.write(&address, bytes);
    auto io = PushMemory(&ram, &registers, &stack_full_ascending);
    io.execute(Instruction(0xff, 0xFFB4, address));
  }

  { // test push register
    auto address = registers.SS.address(registers.BP, 0x0);
    auto bytes = ram.read(&address, sizeof(uint16_t));
    EXPECT_EQ((uint16_t)bytes, 0xE45B);
  }

  { // test push memory
    auto address = registers.SS.address(registers.BP, sizeof(uint16_t));
    auto bytes = ram.read(&address, sizeof(uint16_t));
    EXPECT_EQ((uint16_t)bytes, 0xA74E);
  }
  EXPECT_EQ(registers.SP, 0x6);
}

TEST(PushFlagsTests, test_execute) {
  auto registers = Registers();
  registers.SP = 0x10;
  registers.FLAGS.set(0xf2);
  auto ram = RAM(512);
  auto io = PushFlags(&ram, &registers, &stack_full_ascending);
  io.execute(Instruction(0xff, 0xCD));
  auto address = Address((uint16_t)0x10);
  auto bytes = ram.read(&address, sizeof(uint16_t));
  EXPECT_EQ((uint16_t)bytes, 0xf2);
}

#endif // _PUSH_TESTS_H_
