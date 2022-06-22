#ifndef _MOV_TESTS_H_
#define _MOV_TESTS_H_

#include "logger.h"
#include "mov.h"
#include "ram.h"
#include "gtest/gtest.h"

struct MovRegisterRegisterTests : ::testing::Test {
  struct MovRegisterRegisterT : public MovRegisterRegister {
    MovRegisterRegisterT(Registers *registers)
        : MovRegisterRegister(registers) {}
    using MovRegisterRegister::_IOReader;
    using MovRegisterRegister::_IOWriter;
  };
};

TEST_F(MovRegisterRegisterTests, test__IOReader_reader) {
  auto registers = Registers();
  auto io_reader = MovRegisterRegisterT::_IOReader(&registers);
  auto io = io_reader.reader(Instruction(0xff, 0x8BC3));
  EXPECT_EQ(&registers.BX, io);
}

TEST_F(MovRegisterRegisterTests, test__IOWriter_writer) {
  auto registers = Registers();
  auto io_writer = MovRegisterRegisterT::_IOWriter(&registers);
  auto io = io_writer.writer(Instruction(0xff, 0x8BC3));
  EXPECT_EQ(&registers.AX, io);
}

TEST_F(MovRegisterRegisterTests, test_execute) {
  auto registers = Registers();
  registers.BX += 0x56;
  auto io = MovRegisterRegister(&registers);
  io.execute(Instruction(0xff, 0x8BC3));
  EXPECT_EQ((uint16_t)registers.AX, 0x56);
}

TEST_F(MovRegisterRegisterTests, test_execute_2) {
  auto registers = Registers();
  registers.BX += 0x56;
  auto io = MovRegisterRegister(&registers);
  io.execute(Instruction(0xff, 0x89D8));
  EXPECT_EQ((uint16_t)registers.AX, 0x56);
}

TEST(MovRegisterImmediateTests, test_execute) {
  auto registers = Registers();
  auto io = MovRegisterImmediate(&registers);
  io.execute(Instruction(0xff, 0xB8, 0x0U, 0x0025));
  EXPECT_EQ((uint16_t)registers.AX, 0x25);
}

TEST(MovRegisterImmediateTests, test_execute_2) {
  auto registers = Registers();
  auto io = MovRegisterImmediate(&registers);
  io.execute(Instruction(0xff, 0xB1, 0x0U, 0xF2));
  EXPECT_EQ((uint16_t)registers.AX, 0xF2);
}

TEST(MovRegisterMemoryTests, test_execute) {
  auto registers = Registers();
  registers.DX += 0x12;
  auto ram = RAM(512);
  auto io = MovRegisterMemory(&registers, &ram);
  auto address = Address((uint16_t)0x0100);
  io.execute(Instruction(0xff, 0x8996, (uint16_t)address));
  auto bytes = ram.read(&address, sizeof(uint16_t));
  EXPECT_EQ((uint16_t)bytes, 0x12);
}

TEST(MovRegisterSegmentTests, test_execute) {
  auto registers = Registers();
  registers.BX += 0x12;
  auto io = MovRegisterSegment(&registers);
  io.execute(Instruction(0xff, 0x8EDB));
  EXPECT_EQ(registers.DS, 0x12);
}

TEST(MovRegisterSegmentTests, test_execute_2) {
  auto registers = Registers();
  registers.DS += 0x12;
  auto io = MovRegisterSegment(&registers);
  io.execute(Instruction(0xff, 0x8CDB));
  EXPECT_EQ(registers.BX, 0x12);
}

TEST(MovMemorySegmentTests, test_execute) {
  auto registers = Registers();
  auto ram = RAM(64);
  uint16_t val = 0x12;
  auto bytes = Bytes((uint8_t *)&val, sizeof(uint16_t));
  auto address = Address((uint16_t)(0x010));
  ram.write(&address, bytes);
  auto io = MovMemorySegment(&ram, &registers);
  io.execute(Instruction(0xff, 0x8E90, address));
  EXPECT_EQ((uint16_t)registers.SS, val);
}

TEST(MovMemorySegmentTests, test_execute_2) {
  auto registers = Registers();
  auto ram = RAM(64);
  uint16_t val = 0x12;
  registers.SS += val;
  auto address = Address((uint16_t)(0x010));
  auto io = MovMemorySegment(&ram, &registers);
  io.execute(Instruction(0xff, 0x8C90, address));
  auto bytes = ram.read(&address, sizeof(uint16_t));
  EXPECT_EQ((uint16_t)bytes, val);
}

TEST(MovMemorySegmentTests, test_execute_with_bx_si_set) {
  auto registers = Registers();
  registers.BX = 0x2;
  registers.SI = 0x3;
  auto ram = RAM(64);
  uint16_t val = 0x12;
  auto bytes = Bytes((uint8_t *)&val, sizeof(uint16_t));
  auto base_addr = Address((uint16_t)(0x010));
  auto address = base_addr + (uint16_t)0x5;
  ram.write(&address, bytes);
  auto io = MovMemorySegment(&ram, &registers);
  io.execute(Instruction(0xff, 0x8E90, base_addr));
  EXPECT_EQ((uint16_t)registers.SS, val);
}

#endif // _MOV_TESTS_H_