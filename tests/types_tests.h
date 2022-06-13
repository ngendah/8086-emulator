#ifndef TYPES_TESTS_H_
#define TYPES_TESTS_H_

#include "types.h"
#include "gtest/gtest.h"

TEST(InstructionOpcodeTest, test_sop_t_cast) {
	auto instruction = Instruction().sop(0x45);
	EXPECT_EQ(0x45, (sop_t)instruction);
}

TEST(InstructionOpcodeTest, test_opcode_d_w_t_cast) {
	auto instruction = Instruction().opcode(0x45);
	EXPECT_EQ(0x45, (uint8_t)instruction.opcode_to<opcode_d_w_t>());
}

TEST(InstructionOpcodeTest, test_mod_reg_rm_t_cast) {
	auto instruction = Instruction().mode(0x45);
	EXPECT_EQ(0x45, (uint8_t)instruction.mode_to<mod_reg_rm_t>());
}

TEST(RegisterTest, test_uint16_cast) {
	auto _register = Register(0x0138);
	EXPECT_EQ((uint16_t )_register, (uint16_t )0x0138);
}

TEST(RegisterTest, test_read_hi) {
	auto _register = Register(0x0138);
	EXPECT_EQ(_register.read_hi(), (uint8_t )0x01);
}

TEST(RegisterTest, test_read_lo) {
	auto _register = Register(0x0138);
	EXPECT_EQ(_register.read_lo(), (uint8_t )0x38);
}

TEST(RegisterTest, test_add_offset) {
	auto _register = Register(0x0138) + 0x1;
	EXPECT_EQ((uint16_t )_register, (uint16_t )0x0139);
}

TEST(SegmentTest, test_segment_address_2) {
	auto segment = Segment(0x2345);
	auto reg = Register(0x123c);
	auto addr = segment.address(reg);
	EXPECT_EQ((uint32_t )addr, 0x2468c);
}

#endif /* TYPES_TESTS_H_ */
