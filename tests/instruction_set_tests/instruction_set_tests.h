#ifndef _INSTRUCTION_SET_TESTS_H_
#define _INSTRUCTION_SET_TESTS_H_

#include "instruction_set.h"
#include "mov.h"
#include "push.h"
#include "gtest/gtest.h"
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

#endif // _INSTRUCTION_SET_TESTS_H_
