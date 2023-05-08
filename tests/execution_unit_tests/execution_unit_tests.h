#ifndef _INSTRUCTION_EXECUTOR_TESTS_H_
#define _INSTRUCTION_EXECUTOR_TESTS_H_

#include "execution_unit.h"
#include "lea.h"
#include "mov.h"
#include "ram.h"
#include "gtest/gtest.h"
#include <fstream>
#include <typeinfo>

const std::string _file_path(std::string const &file) {
  auto file_path = fmt::format("{}", __FILE__);
  auto idx = file_path.find_last_of("/\\");
  auto dir = file_path.substr(0, idx);
  return fmt::format("{}/{}", dir, file);
}

TEST(ExecutionUnit, test_fetch_decode) {
  std::stringstream instructions(std::ios_base::trunc | std::ios_base::in |
                                 std::ios_base::out | std::ios_base::binary);
  uint8_t data[7] = {0x3e, 0xc7, 0x86, 0x46, 0xf2, 0x34, 0x12};
  instructions.write((char *)&data, sizeof(data));
  auto ram = RAM(125);
  auto registers = Registers();
  auto executor = ExecutionUnit(&ram, &registers);
  executor.bootstrap(instructions.rdbuf(), true);
  auto fetch = executor.fetch();
  auto code = fetch.first;
  EXPECT_EQ(code, 0xc7);
  auto instruction = fetch.second;
  EXPECT_EQ(instruction.sop(), 0x3e);
  EXPECT_EQ(instruction.opcode_to<uint8_t>(), 0xc7);
  EXPECT_EQ(instruction.mode_to<uint8_t>(), 0x86);
  EXPECT_EQ(instruction.offset(), 0xf246);
  EXPECT_EQ(instruction.data<uint16_t>(), 0x1234);
  // decode
  auto micro_op = executor.decode(code);
  EXPECT_NE(&micro_op, nullptr);
}

TEST(ExecutionUnit, test_fetch_decode_execute_1) {
  std::fstream executable(_file_path("asm/mov1.bin"),
                          std::ios_base::in | std::ios_base::binary);
  EXPECT_EQ(executable.is_open(), true);
  auto ram = RAM(125);
  auto registers = Registers();
  auto executor = ExecutionUnit(&ram, &registers);
  executor.bootstrap(executable.rdbuf(), true);
  {
    auto fetch = executor.fetch();
    EXPECT_EQ(fetch.first, 0xb4);
    auto instruction = fetch.second;
    EXPECT_EQ(instruction.sop(), 0xff);
    EXPECT_EQ(instruction.opcode_to<uint8_t>(), 0xb4);
    EXPECT_EQ(instruction.mode_to<uint8_t>(), 0x0);
    EXPECT_EQ(instruction.offset(), 0x0);
    EXPECT_EQ(instruction.data<uint16_t>(), 0x9);
    auto microop = executor.decode(fetch.first);
    EXPECT_EQ(typeid(*microop), typeid(MovRegisterImmediate));
    microop->execute(instruction);
    EXPECT_EQ((uint8_t)registers.AX, 9);
  }
  {
    auto fetch = executor.fetch();
    EXPECT_EQ(fetch.first, 0xb8);
    auto instruction = fetch.second;
    EXPECT_EQ(instruction.sop(), 0xff);
    EXPECT_EQ(instruction.opcode_to<uint8_t>(), 0xb8);
    EXPECT_EQ(instruction.mode_to<uint8_t>(), 0x0);
    EXPECT_EQ(instruction.offset(), 0x0);
    EXPECT_EQ(instruction.data<uint16_t>(), 0x4c00);
    auto microop = executor.decode(fetch.first);
    EXPECT_EQ(typeid(*microop), typeid(MovRegisterImmediate));
    microop->execute(instruction);
    EXPECT_EQ((uint16_t)registers.AX, 0x4c00);
  }
}

TEST(ExecutionUnit, test_fetch_decode_execute_2) {
  std::fstream executable(_file_path("asm/mov2.bin"),
                          std::ios_base::in | std::ios_base::binary);
  EXPECT_EQ(executable.is_open(), true);
  auto ram = RAM(125);
  auto registers = Registers();
  auto executor = ExecutionUnit(&ram, &registers);
  executor.bootstrap(executable.rdbuf(), true);
  {
    auto fetch = executor.fetch();
    EXPECT_EQ(fetch.first, 0x8d);
    auto instruction = fetch.second;
    EXPECT_EQ(instruction.sop(), 0xff);
    EXPECT_EQ(instruction.opcode_to<uint8_t>(), 0x8d);
    EXPECT_EQ(instruction.offset(), 0x10c);
    EXPECT_EQ(instruction.data<uint16_t>(), 0x0);
    auto microop = executor.decode(fetch.first);
    EXPECT_EQ(typeid(*microop), typeid(LEA));
    microop->execute(instruction);
    EXPECT_EQ((uint16_t)registers.AX, 0x10c);
  }
  {
    auto fetch = executor.fetch();
    EXPECT_EQ(fetch.first, 0x8e);
    auto instruction = fetch.second;
    EXPECT_EQ(instruction.opcode_to<uint8_t>(), 0x8e);
    EXPECT_EQ(instruction.mode_to<uint8_t>(), 0xd8);
    auto microop = executor.decode(fetch.first);
    EXPECT_EQ(typeid(*microop), typeid(MovSegmentAndRegisterMemory));
    microop->execute(instruction);
    EXPECT_EQ((uint16_t)registers.DS, 0x10c);
  }
}

#endif // _INSTRUCTION_EXECUTOR_TESTS_H_
