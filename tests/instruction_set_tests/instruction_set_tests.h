#ifndef _INSTRUCTION_SET_TESTS_H_
#define _INSTRUCTION_SET_TESTS_H_

#include "instruction_set.h"
#include "mov.h"
#include "gtest/gtest.h"
#include <list>

struct Compare {
  template <typename T1, typename T2>
  const T2 &operator()(const std::pair<T1, T2> &key_val) const {
    return key_val.second;
  }
};

TEST(InstructionSetTests, test_decode_1) {
  InstructionSet instructions;
  auto cls_create = instructions.instructions.find(MicroOp::Key(0x8B))->second;
  EXPECT_EQ(cls_create, &MovRegisterAndMemory::create);
  auto micro_op = instructions.decode(0x8B, MicroOp::Params());
  EXPECT_NE(micro_op.get(), nullptr);
}

#endif // _INSTRUCTION_SET_TESTS_H_
