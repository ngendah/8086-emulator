#ifndef _IN_TESTS_H_
#define _IN_TESTS_H_

#include "in.h"
#include "logger.h"
#include "ram.h"

#include "gtest/gtest.h"

TEST(INPortTests, test_execute) {
  auto ram = RAM(128);
  auto registers = Registers();
  uint8_t port_number = 53;
  auto port = Port();
  port.to_u8().write(0xBA);
  registers.PORTS.add(port_number, &port);
  auto instruction = Instruction(0xff, 0xE400, 0x0, port_number);
  auto io = INPort(&ram, &registers);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.AX, port.to_u8().read());
}

TEST(INDXTests, test_execute) {
  auto ram = RAM(12);
  auto registers = Registers();
  uint8_t port_number = 53;
  auto port = Port();
  port.to_u8().write(0xBA);
  registers.PORTS.add(port_number, &port);
  registers.DX.write(port_number);
  auto instruction = Instruction(0xff, 0xEC);
  auto io = INDX(&ram, &registers);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.AX, port.to_u8().read());
}

#endif // _IN_TESTS_H_
