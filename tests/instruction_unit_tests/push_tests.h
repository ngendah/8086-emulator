#ifndef _PUSH_TESTS_H_
#define _PUSH_TESTS_H_

#include "logger.h"
#include "push.h"
#include "ram.h"

#include "gtest/gtest.h"
#include <array>

TEST(PushRegisterTests, test_execute) {
  auto registers = Registers();
  registers.SS = 0x10;
  registers.SI = 0x37;
  auto ram = RAM(512);
  auto io = PushRegister(&ram, &registers);
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
  auto io = PushMemory(&ram, &registers);
  io.execute(Instruction(0xff, 0xFFB4, address));
  {
    address = Address((uint16_t)0x100);
    bytes = ram.read(&address, sizeof(uint16_t));
    EXPECT_EQ((uint16_t)bytes, 0x37);
  }
}

TEST(PushSegmentTests, test_execute) {
  auto registers = Registers();
  registers.SS = 0x10;
  registers.DS = 0x37;
  auto ram = RAM(512);
  auto io = PushSegment(&ram, &registers);
  io.execute(Instruction(0xff, 0x1E00));
  auto address = Address((uint16_t)0x100);
  auto bytes = ram.read(&address, sizeof(uint16_t));
  EXPECT_EQ((uint16_t)bytes, 0x37);
}

TEST(PushTests, test_execute) {
  const auto size = 512u;
  std::array<uint8_t, size> buffer{};
  auto ram = RAM(&buffer.at(0), size);
  auto registers = Registers();
  registers.SS = 0x10;
  registers.BP = 0x2;
  registers.SP = 0x2;
  {
    registers.BX = 0xE45B;
    auto io = PushRegister(&ram, &registers);
    io.execute(Instruction(0xff, 0x5300));
  }

  {
    uint16_t val = 0xA74E;
    auto address = Address((uint16_t)0x0083);
    auto bytes = Bytes((uint8_t *)&val, sizeof(uint16_t));
    ram.write(&address, bytes);
    auto io = PushMemory(&ram, &registers);
    io.execute(Instruction(0xff, 0xFFB4, address));
  }

  { // test push register
    // TODO use SP
    auto address = registers.SS.address(0x2) - (uint16_t)1;
    const auto val = ram.read_u16(&address);
    EXPECT_EQ(val, 0x5BE4);
  }

  { // test push memory
    // TODO use SP
    auto address = registers.SS.address(0) - (uint16_t)1;
    const auto val = ram.read_u16(&address);
    EXPECT_EQ(val, 0x4EA7);
  }
}

TEST(PushFlagsTests, test_execute) {
  auto registers = Registers();
  registers.SP = 0x10;
  registers.FLAGS.set(0xf2);
  auto ram = RAM(512);
  auto io = PushFlags(&ram, &registers);
  io.execute(Instruction(0xff, 0xCD));
  auto address = Address((uint16_t)0x10);
  auto bytes = ram.read(&address, sizeof(uint16_t));
  EXPECT_EQ((uint16_t)bytes, 0xf2);
}

TEST(StackFullDescendingTests, test_next) {
  const auto buffer_size = 30u;
  std::array<uint8_t, buffer_size> buffer1{}, buffer2{};
  RAM ram1{&buffer1.at(0), buffer_size}, ram2{&buffer2.at(0), buffer_size};
  BUSIO src{&ram1, Address((uint8_t)15)}, dest{&ram2, Address((uint8_t)25)};
  src.to_u16().write(make_word(0x11, 0x55));
  auto registers = Registers();
  registers.SP = buffer_size - 1;
  auto params = OpType::Params{word, reinterpret_cast<IO *>(&src),
                               reinterpret_cast<IO *>(&dest), &registers};
  StackFullDescending::next(params);
  EXPECT_EQ(buffer2.at(25), 0x55);
  EXPECT_EQ(buffer2.at(24), 0x11);
}

TEST(StackFullDescendingTests, test_prev) {
  const auto buffer_size = 30u;
  std::array<uint8_t, buffer_size> buffer1{}, buffer2{};
  RAM ram1{&buffer1.at(0), buffer_size}, ram2{&buffer2.at(0), buffer_size};
  const auto src_addr = Address((uint16_t)24);
  BUSIO src{&ram1, src_addr}, dest{&ram2, Address((uint8_t)15)};
  src.to_u8().write(0x11);
  src.set_address(src_addr + (uint16_t)1);
  src.to_u8().write(0x55);
  src.set_address(src_addr);
  auto registers = Registers();
  registers.SP = buffer_size - 1;
  auto params = OpType::Params{word, reinterpret_cast<IO *>(&src),
                               reinterpret_cast<IO *>(&dest), &registers};
  StackFullDescending::prev(params);
  EXPECT_EQ(dest.to_u16().read(), make_word(0x11, 0x55));
}

#endif // _PUSH_TESTS_H_
