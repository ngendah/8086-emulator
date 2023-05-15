#ifndef _EXECUTION_UNIT_TESTS_1_H
#define _EXECUTION_UNIT_TESTS_1_H

#include "execution_unit.h"
#include "lea.h"
#include "mov.h"
#include "ram.h"

#include "gtest/gtest.h"
#include <array>
#include <fstream>

extern const std::string _file_path(std::string const &file);

TEST(ExecutionUnit, test_fetch_decode_execute_10) {
  std::fstream executable(_file_path("asm/mov3.bin"),
                          std::ios_base::in | std::ios_base::binary);
  EXPECT_EQ(executable.is_open(), true);
  std::array<uint8_t, 2048> buffer{};
  auto ram = RAM(&buffer.at(0), 2048);
  auto registers = Registers();
  auto executor = ExecutionUnit(&ram, &registers);
  executor.bootstrap(executable.rdbuf(), true);
  {
    auto fetch = executor.fetch();
    auto instruction = fetch.second;
    auto microop = executor.decode(fetch.first);
    microop->execute(instruction);
    EXPECT_EQ((uint16_t)registers.AX, 0x40);
  }
  {
    auto fetch = executor.fetch();
    auto instruction = fetch.second;
    auto microop = executor.decode(fetch.first);
    microop->execute(instruction);
    EXPECT_EQ((uint16_t)registers.DS, (uint8_t)registers.AX);
  }
  {
    auto fetch = executor.fetch();
    auto instruction = fetch.second;
    auto microop = executor.decode(fetch.first);
    microop->execute(instruction);
    EXPECT_EQ(buffer.at(1138), 0x34);
  }
}

#endif // _EXECUTION_UNIT_TESTS_1_H
