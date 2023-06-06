#ifndef _INSTRUCTION_SET_TESTS_H_
#define _INSTRUCTION_SET_TESTS_H_

#include "instruction_unit.h"
#include "mov.h"
#include "push.h"
#include "ram.h"

#include "gtest/gtest.h"
#include <fstream>
#include <list>
#include <typeinfo>

struct Compare {
  template <typename T1, typename T2>
  const T2 &operator()(const std::pair<T1, T2> &key_val) const {
    return key_val.second;
  }
};

TEST(InstructionUnitTests, test_decode) {
  InstructionUnit instructions;
  auto cls_create = instructions.find(MicroOp::Key(0x8B));
  EXPECT_EQ(cls_create, &MovRegisterAndMemory::create);
  auto micro_op = instructions.decode(0x8B, MicroOp::Params());
  EXPECT_NE(micro_op.get(), nullptr);
}

TEST(InstructionUnitTests, test_decode_push_segment) {
  InstructionUnit instructions;
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

TEST(InstructionUnitTests, test_decode_push_register) {
  InstructionUnit instructions;
  for (int key = 0x50; key < 0x58; key++) {
    auto cls_create = instructions.find(MicroOp::Key(key));
    EXPECT_EQ(cls_create, &PushRegister::create);
  }
}

TEST(InstructionUnitTests, test_find_from_map) {
  InstructionUnit instructions;
  auto ins_code = instructions.find(0x88);
  EXPECT_NE(ins_code, nullptr);
  EXPECT_EQ(ins_code->_memonic, "MOV");
}

TEST(InstructionCodeTests, test_cnstr) {
  auto code = InstructionCode("ADD", "E", "E");
  EXPECT_EQ(code._memonic, "ADD");
  EXPECT_EQ(code._arguments.size(), 2);
}

TEST(InstructionCodeTests, test_has_mode_1) {
  auto code = InstructionCode("ADD", "E", "E");
  EXPECT_EQ(code.has_mod(), true);
}

TEST(InstructionCodeTests, test_has_mode_2) {
  auto code = InstructionCode("INC", "E");
  EXPECT_EQ(code.has_mod(), true);
}

TEST(InstructionCodeTests, test_has_mode_3) {
  auto code = InstructionCode("PUSH");
  EXPECT_EQ(code.has_mod(), false);
}

TEST(InstructionCodeTests, test_has_data_1) {
  auto code = InstructionCode("ADC", "E", "Iw");
  auto data = code.has_data();
  EXPECT_EQ(data.first, true);
  EXPECT_EQ(data.second, sizeof(uint16_t));
}

TEST(InstructionCodeTests, test_has_data_2) {
  auto code = InstructionCode("RET", "Iw");
  auto data = code.has_data();
  EXPECT_EQ(data.first, true);
  EXPECT_EQ(data.second, sizeof(uint16_t));
}

TEST(InstructionCodeTests, test_has_data_3) {
  auto code = InstructionCode("MOV", "E", "Ib");
  auto data = code.has_data();
  EXPECT_EQ(data.first, true);
  EXPECT_EQ(data.second, sizeof(uint8_t));
}

TEST(InstructionCodeTests, test_has_data_4) {
  auto code = InstructionCode("INT", "Ib");
  auto data = code.has_data();
  EXPECT_EQ(data.first, true);
  EXPECT_EQ(data.second, sizeof(uint8_t));
}

TEST(InstructionCodeTests, test_has_data_5) {
  auto code = InstructionCode();
  auto data = code.has_data();
  EXPECT_EQ(data.first, false);
  EXPECT_EQ(data.second, 0);
}

TEST(InstructionCodeTests, test_has_disp_1) {
  auto code = InstructionCode("ADD", "E", "E");
  auto disp = code.has_disp((uint8_t)0x2 << 6);
  EXPECT_EQ(disp.first, true);
  EXPECT_EQ(disp.second, sizeof(uint16_t));
}

TEST(InstructionCodeTests, test_has_disp_2) {
  auto code = InstructionCode("ADD", "E", "E");
  auto disp = code.has_disp((uint8_t)0x1 << 6);
  EXPECT_EQ(disp.first, true);
  EXPECT_EQ(disp.second, sizeof(uint8_t));
}

TEST(InstructionCodeTests, test_has_disp_3) {
  auto code = InstructionCode("ADD", "E", "E");
  auto disp = code.has_disp(0x0);
  EXPECT_EQ(disp.first, false);
  EXPECT_EQ(disp.second, 0);
}

#endif // _INSTRUCTION_SET_TESTS_H_
