#ifndef _MOV_TESTS_H_
#define _MOV_TESTS_H_

#include "mov.h"
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

#endif // _MOV_TESTS_H_