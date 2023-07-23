#ifndef _IN_TESTS_H_
#define _IN_TESTS_H_

#include "bytes.h"
#include "in.h"
#include "logger.h"
#include "ram.h"

#include "gtest/gtest.h"

TEST(INPortTests, test_execute) {
  const uint8_t port_number = 53;
  auto ram = RAM(128);
  auto device = RAM(sizeof_ui16);
  auto addr = Address();
  auto val = (uint8_t)0xBA;
  device.write(&addr, Bytes(&val, 1));
  auto bus = BUS(Devices::devices_t{{0, &ram}, {port_number, &device}});
  auto registers = Registers();
  auto instruction = Instruction(0xff, 0xE400, 0x0, port_number);
  auto io = INPort(&bus, &registers);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.AX, val);
}

TEST(INDXTests, test_execute) {
  const uint8_t port_number = 53;
  auto ram = RAM(12);
  auto device = RAM(sizeof_ui16);
  auto addr = Address();
  auto val = (uint8_t)0xBA;
  device.write(&addr, Bytes(&val, 1));
  auto bus = BUS(Devices::devices_t{{0, &ram}, {port_number, &device}});
  auto registers = Registers();
  registers.DX.write(port_number);
  auto instruction = Instruction(0xff, 0xEC);
  auto io = INDX(&bus, &registers);
  io.execute(instruction);
  EXPECT_EQ((uint16_t)registers.AX, val);
}

#endif // _IN_TESTS_H_
