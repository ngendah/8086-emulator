#ifndef _MOV_TESTS_H_
#define _MOV_TESTS_H_

#include "decoders.h"
#include "logger.h"
#include "mov.h"
#include "ram.h"

#include "gtest/gtest.h"

struct MovRegisterRegisterTests : ::testing::Test {
  struct MovRegisterRegisterT : public MovRegisterRegister {
    MovRegisterRegisterT(Registers *registers)
        : MovRegisterRegister(nullptr, registers) {}
  };
};

TEST_F(MovRegisterRegisterTests, test__IOReader_reader) {
  auto registers = Registers();
  auto io_reader = RR_Decoder(nullptr, &registers);
  auto io = io_reader.source(Instruction(0xff, 0x8BC3));
  EXPECT_EQ(&registers.BX, io);
}

TEST_F(MovRegisterRegisterTests, test__IOWriter_writer) {
  auto registers = Registers();
  auto io_writer = RR_Decoder(nullptr, &registers);
  auto io = io_writer.destination(Instruction(0xff, 0x8BC3));
  EXPECT_EQ(&registers.AX, io);
}

TEST_F(MovRegisterRegisterTests, test_execute) {
  auto registers = Registers();
  registers.BX += 0x56;
  auto io = MovRegisterRegister(nullptr, &registers);
  io.execute(Instruction(0xff, 0x8BC3));
  EXPECT_EQ((uint16_t)registers.AX, 0x56);
}

TEST_F(MovRegisterRegisterTests, test_execute_2) {
  auto registers = Registers();
  registers.BX += 0x56;
  auto io = MovRegisterRegister(nullptr, &registers);
  io.execute(Instruction(0xff, 0x89D8));
  EXPECT_EQ((uint16_t)registers.AX, 0x56);
}

TEST(MovRegisterImmediateTests, test_execute) {
  auto registers = Registers();
  auto io = MovRegisterImmediate(nullptr, &registers);
  io.execute(Instruction(0xff, 0xB8, 0x0U, (uint16_t)0x0025));
  EXPECT_EQ((uint16_t)registers.AX, 0x25);
}

TEST(MovRegisterImmediateTests, test_execute_2) {
  auto registers = Registers();
  auto io = MovRegisterImmediate(nullptr, &registers);
  io.execute(Instruction(0xff, 0xB1, 0x0U, make_word(0x0, 0xF2)));
  EXPECT_EQ((uint16_t)registers.AX, 0xF2);
}

TEST(MovRegisterMemoryTests, test_execute) {
  auto registers = Registers();
  registers.DX += 0x12;
  auto ram = RAM(512);
  auto bus = BUS::from_device(&ram);
  auto io = MovRegisterMemory(&bus, &registers);
  auto address = Address((uint16_t)0x0100);
  io.execute(Instruction(0xff, 0x8996, (uint16_t)address));
  auto bytes = ram.read(&address, sizeof_ui16);
  EXPECT_EQ((uint16_t)bytes, 0x12);
}

TEST(MovRegisterSegmentTests, test_execute) {
  auto registers = Registers();
  registers.BX += 0x12;
  auto io = MovRegisterSegment(nullptr, &registers);
  io.execute(Instruction(0xff, 0x8EDB));
  EXPECT_EQ(registers.DS, 0x12);
}

TEST(MovRegisterSegmentTests, test_execute_2) {
  auto registers = Registers();
  registers.DS += 0x12;
  auto io = MovRegisterSegment(nullptr, &registers);
  io.execute(Instruction(0xff, 0x8CDB));
  EXPECT_EQ(registers.BX, 0x12);
}

TEST(MovMemorySegmentTests, test_execute) {
  auto registers = Registers();
  auto ram = RAM(64);
  auto bus = BUS::from_device(&ram);
  uint16_t val = 0x12;
  auto bytes = Bytes((uint8_t *)&val, sizeof_ui16);
  auto address = Address((uint16_t)(0x010));
  ram.write(&address, bytes);
  auto io = MovMemorySegment(&bus, &registers);
  io.execute(Instruction(0xff, 0x8E90, address));
  EXPECT_EQ((uint16_t)registers.SS, val);
}

TEST(MovMemorySegmentTests, test_execute_2) {
  auto registers = Registers();
  auto ram = RAM(64);
  auto bus = BUS::from_device(&ram);
  uint16_t val = 0x12;
  registers.SS += val;
  auto address = Address((uint16_t)(0x010));
  auto io = MovMemorySegment(&bus, &registers);
  io.execute(Instruction(0xff, 0x8C90, address));
  auto bytes = ram.read(&address, sizeof_ui16);
  EXPECT_EQ((uint16_t)bytes, val);
}

TEST(MovMemorySegmentTests, test_execute_with_bx_si_set) {
  auto registers = Registers();
  registers.BX = 0x2;
  registers.SI = 0x3;
  auto ram = RAM(64);
  auto bus = BUS::from_device(&ram);
  uint16_t val = 0x12;
  auto bytes = Bytes((uint8_t *)&val, sizeof_ui16);
  auto base_addr = Address((uint16_t)(0x010));
  auto address = base_addr + (uint16_t)0x5;
  ram.write(&address, bytes);
  auto io = MovMemorySegment(&bus, &registers);
  io.execute(Instruction(0xff, 0x8E90, base_addr));
  EXPECT_EQ((uint16_t)registers.SS, val);
}

TEST(MovAccumulatorTests, test_execute) {
  auto registers = Registers();
  auto ram = RAM(64);
  auto bus = BUS::from_device(&ram);
  uint16_t val = 0x23;
  auto bytes = Bytes((uint8_t *)&val, sizeof_ui16);
  auto address = Address((uint16_t)(0x010));
  ram.write(&address, bytes);
  PLOGD << fmt::format("source=0x{:x}", (long)&bus);
  auto io = MovAccumulator(&bus, &registers);
  io.execute(Instruction(0xff, 0xA100, address));
  PLOGD << fmt::format("destination=0x{:x}", (long)&registers.AX);
  EXPECT_EQ((uint16_t)registers.AX, val);
}

TEST(MovMemoryImmediateTests, test_execute) {
  auto registers = Registers();
  auto ram = RAM(64);
  auto bus = BUS::from_device(&ram);
  uint16_t val = 0x2310;
  uint16_t offset = 0x10;
  auto io = MovMemoryImmediate(&bus, &registers);
  io.execute(Instruction(0x3E, 0xC786, offset, val));
  auto address = registers.DS.address(offset);
  auto bytes = ram.read(&address, sizeof_ui16);
  EXPECT_EQ((uint16_t)bytes, val);
}

#endif // _MOV_TESTS_H_
