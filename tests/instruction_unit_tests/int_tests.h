#ifndef intr_TESTS_H_
#define intr_TESTS_H_

#include "int.h"
#include "iret.h"
#include "logger.h"
#include "ram.h"

#include "gtest/gtest.h"
#include <array>

TEST(INTTests, test_before_execute) {
  std::array<uint8_t, 125> buffer{};
  auto ram = RAM(&buffer.at(0), 125);
  auto bus = BUS::from_device(&ram);
  auto addr_latch = AddressLatch(&bus);
  auto registers = Registers();
  registers.SP = 0x25;
  {
    registers.FLAGS.set(0xFFFF);
    registers.CS = 0x35FF;
    registers.IP = 0xEA55;
    auto intr = INT(&bus, &registers);
    intr.before_execute(Instruction());
  }
  // clear values
  {
    registers.FLAGS.set(0);
    registers.CS = 0;
    registers.IP = 0;
  }
  // read back from stack
  {
    auto address =
        PhysicalAddresser(&registers).address(&registers.SS, registers.SP);
    addr_latch.set_address(address);
    auto params = OpType::Params{word, &addr_latch, &registers.IP, &registers};
    StackFullDescending::pop(params);
    EXPECT_EQ((uint16_t)registers.IP, 0xEA55);
  }
  {
    auto address =
        PhysicalAddresser(&registers).address(&registers.SS, registers.SP);
    addr_latch.set_address(address);
    auto params = OpType::Params{word, &addr_latch, &registers.CS, &registers};
    StackFullDescending::pop(params);
    EXPECT_EQ((uint16_t)registers.CS, 0x35FF);
  }
  {
    auto address =
        PhysicalAddresser(&registers).address(&registers.SS, registers.SP);
    addr_latch.set_address(address);
    auto params =
        OpType::Params{word, &addr_latch, &registers.FLAGS, &registers};
    StackFullDescending::pop(params);
    EXPECT_EQ((uint16_t)registers.FLAGS, 0xFFFF);
  }
}

TEST(INTTests, test_execute) {
  const uint16_t interrupt_number = 3; // Breakpoint interrupt
  std::array<uint8_t, 25> buffer{};
  auto ram = RAM(&buffer.at(0), 25);
  auto bus = BUS::from_device(&ram);
  auto ip_addr = Address((uint16_t)(3 * 4));
  uint16_t ip_word = 0xEADD;
  ram.write(&ip_addr, Bytes((uint8_t *)&ip_word, 2));
  auto cs_addr = ip_addr + (uint16_t)2;
  uint16_t cs_word = 0xAEDD;
  ram.write(&cs_addr, Bytes((uint8_t *)&cs_word, 2));
  auto registers = Registers();
  auto intr = INT(&bus, &registers);
  auto instruction = Instruction(0xff, 0xCD, 0x0, interrupt_number);
  intr.execute(instruction);
  EXPECT_EQ((uint16_t)registers.CS, cs_word);
  EXPECT_EQ((uint16_t)registers.IP, ip_word);
}

#endif // intr_TESTS_H_
