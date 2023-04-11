#ifndef _INSTRUCTION_SET_TESTS_H_
#define _INSTRUCTION_SET_TESTS_H_

#include "instruction_set.h"
#include "mov.h"
#include "push.h"
#include "ram.h"
#include "gtest/gtest.h"
#include <fstream>
#include <list>

struct Compare {
  template <typename T1, typename T2>
  const T2 &operator()(const std::pair<T1, T2> &key_val) const {
    return key_val.second;
  }
};

TEST(InstructionSetTests, test_decode) {
  InstructionSet instructions;
  auto cls_create = instructions.find(MicroOp::Key(0x8B));
  EXPECT_EQ(cls_create, &MovRegisterAndMemory::create);
  auto micro_op = instructions.decode(0x8B, MicroOp::Params());
  EXPECT_NE(micro_op.get(), nullptr);
}

TEST(InstructionSetTests, test_decode_push_segment) {
  InstructionSet instructions;
  {
    auto cls_create = instructions.find(MicroOp::Key(0x06));
    EXPECT_EQ(cls_create, &PushSegment::create);
  }
  {
    auto cls_create = instructions.find(MicroOp::Key(0x0E));
    EXPECT_EQ(cls_create, &PushSegment::create);
  }
  {
    auto cls_create = instructions.find(MicroOp::Key(0x16));
    EXPECT_EQ(cls_create, &PushSegment::create);
  }
  {
    auto cls_create = instructions.find(MicroOp::Key(0x06));
    EXPECT_EQ(cls_create, &PushSegment::create);
  }
  auto cls_create = instructions.find(MicroOp::Key(0x1E));
  EXPECT_EQ(cls_create, &PushSegment::create);
}

TEST(InstructionSetTests, test_decode_push_register) {
  InstructionSet instructions;
  for (int key = 0x50; key < 0x58; key++) {
    auto cls_create = instructions.find(MicroOp::Key(key));
    EXPECT_EQ(cls_create, &PushRegister::create);
  }
}

TEST(InstructionSetTests, test_find_from_map) {
    InstructionSet instructions;
    auto ins_code = instructions.find(0x88);
    EXPECT_NE(ins_code, nullptr);
    EXPECT_EQ(ins_code->_memonic, "MOV");
}

TEST(InstructionCodeTests, test_cnstr) {
  auto code = InstructionCode(0x01, "ADD", "E", "E");
  EXPECT_EQ(code._opcode, 0x01);
  EXPECT_EQ(code._memonic, "ADD");
  EXPECT_EQ(code._arguments.size(), 2);
}

TEST(InstructionCodeTests, test_has_mode_1) {
  auto code = InstructionCode(0x01, "ADD", "E", "E");
  EXPECT_EQ(code.has_mode(), true);
}

TEST(InstructionCodeTests, test_has_mode_2) {
  auto code = InstructionCode(0x40, "INC", "E");
  EXPECT_EQ(code.has_mode(), true);
}

TEST(InstructionCodeTests, test_has_mode_3) {
  auto code = InstructionCode(0x16, "PUSH");
  EXPECT_EQ(code.has_mode(), false);
}

TEST(InstructionCodeTests, test_has_data_1) {
  auto code = InstructionCode(0x16, "ADC", "E", "Iw");
  auto data = code.has_data();
  EXPECT_EQ(data.first, true);
  EXPECT_EQ(data.second, sizeof(uint16_t));
}

TEST(InstructionCodeTests, test_has_data_2) {
  auto code = InstructionCode(0xC2, "RET", "Iw");
  auto data = code.has_data();
  EXPECT_EQ(data.first, true);
  EXPECT_EQ(data.second, sizeof(uint16_t));
}

TEST(InstructionCodeTests, test_has_data_3) {
  auto code = InstructionCode(0xC6, "MOV", "E", "Ib");
  auto data = code.has_data();
  EXPECT_EQ(data.first, true);
  EXPECT_EQ(data.second, sizeof(uint8_t));
}

TEST(InstructionCodeTests, test_has_data_4) {
  auto code = InstructionCode(0xCD, "INT", "Ib");
  auto data = code.has_data();
  EXPECT_EQ(data.first, true);
  EXPECT_EQ(data.second, sizeof(uint8_t));
}

TEST(InstructionCodeTests, test_has_data_5) {
  auto code = InstructionCode(0xC9);
  auto data = code.has_data();
  EXPECT_EQ(data.first, false);
  EXPECT_EQ(data.second, 0);
}

TEST(InstructionCodeTests, test_has_disp_1) {
  auto code = InstructionCode(0x03, "ADD", "E", "E");
  auto disp = code.has_disp((uint8_t)0x2 << 6);
  EXPECT_EQ(disp.first, true);
  EXPECT_EQ(disp.second, sizeof(uint16_t));
}

TEST(InstructionCodeTests, test_has_disp_2) {
  auto code = InstructionCode(0x03, "ADD", "E", "E");
  auto disp = code.has_disp((uint8_t)0x1 << 6);
  EXPECT_EQ(disp.first, true);
  EXPECT_EQ(disp.second, sizeof(uint8_t));
}

TEST(InstructionCodeTests, test_has_disp_3) {
  auto code = InstructionCode(0x03, "ADD", "E", "E");
  auto disp = code.has_disp(0x0);
  EXPECT_EQ(disp.first, false);
  EXPECT_EQ(disp.second, 0);
}

TEST(InstructionsExecutor, test_fetch_decode) {
  std::stringstream instructions(std::ios_base::trunc | std::ios_base::in |
                                 std::ios_base::out | std::ios_base::binary);
  uint8_t data[7] = {0x3e, 0xc7, 0x86, 0xf2, 0x46, 0x12, 0x34};
  instructions.write((char *)&data, sizeof(data));
  auto ram = RAM(125);
  auto registers = Registers();
  auto executor = InstructionsExecutor(instructions.rdbuf(), &ram, &registers);
  executor.beg();
  auto fetch = executor.fetch();
  auto code = fetch.first;
  EXPECT_EQ(code->_memonic, "MOV");
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

#endif // _INSTRUCTION_SET_TESTS_H_
