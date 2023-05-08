#ifndef _CMP_TESTS_H_
#define _CMP_TESTS_H_

#include "cmp.h"
#include "logger.h"
#include "ram.h"
#include "gtest/gtest.h"

// TODO fix to correctly test the results

TEST(CMPRegistersTests, test_execute) {
  auto ram = RAM(128);
  auto registers = Registers();
  registers.AX = 5;
  registers.BX = 3;
  auto io = CMPRegisters(&ram, &registers);
  auto instruction = Instruction(0xff, 0x3BC3);
  io.execute(instruction);
  EXPECT_NE((uint16_t)registers.FLAGS, 0);
}

TEST(CMPRegisterMemoryTests, test_execute) {
  auto ram = RAM(128);
  auto address = Address((uint8_t)0x24);
  uint8_t val = 5;
  auto bytes = Bytes(&val, 1);
  ram.write(&address, bytes);
  auto registers = Registers();
  registers.DX = 5;
  auto io = CMPRegisterMemory(&ram, &registers);
  auto instruction = Instruction(0x3E, 0x3996, (uint16_t)address);
  io.execute(instruction);
  EXPECT_NE((uint16_t)registers.FLAGS, 0);
}

TEST(CMPRegisterImmediateTests, test_execute_1) {
  auto ram = RAM(128);
  auto registers = Registers();
  registers.CX = 5;
  auto io = CMPRegisterImmediate(&ram, &registers);
  auto instruction = Instruction(0xff, 0x81F9, 0x0083);
  PLOGD << instruction;
  io.execute(instruction);
  EXPECT_NE((uint16_t)registers.FLAGS, 0);
}

TEST(CMPRegisterImmediateTests, test_execute_2) {
  auto ram = RAM(128);
  auto registers = Registers();
  registers.CX = 5;
  auto io = CMPRegisterImmediate(&ram, &registers);
  auto instruction = Instruction(0xff, 0x80F9, 0x83);
  PLOGD << instruction;
  io.execute(instruction);
  EXPECT_NE((uint16_t)registers.FLAGS, 0);
}

TEST(CMPRegisterImmediateTests, test_execute_3) {
  auto ram = RAM(128);
  auto registers = Registers();
  registers.CX = 5;
  auto io = CMPRegisterImmediate(&ram, &registers);
  auto instruction = Instruction(0xff, 0x83F9, 0x43);
  PLOGD << instruction;
  io.execute(instruction);
  EXPECT_NE((uint16_t)registers.FLAGS, 0);
}

#endif // _CMP_TESTS_H_