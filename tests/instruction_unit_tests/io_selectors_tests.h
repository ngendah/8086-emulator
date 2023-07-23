#ifndef _IO_SELECTOR_TESTS_H_
#define _IO_SELECTOR_TESTS_H_

#include "io_selectors.h"
#include "ram.h"

#include "gtest/gtest.h"

struct RegisterIOSelectorTests : ::testing::Test {
protected:
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct _RegisterSelector2 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().RM;
    }
  };
};

TEST_F(RegisterIOSelectorTests, test_get_1) {
  auto registers = Registers();
  auto selector = _RegisterSelector1();
  auto reg_io_selector = RegisterIOSelector(&registers, &selector);
  auto _register = reg_io_selector.get(Instruction(0xff, (uint16_t)0x8BC3));
  ASSERT_EQ(&registers.AX, _register);
}

TEST_F(RegisterIOSelectorTests, test_get_2) {
  auto registers = Registers();
  auto selector = _RegisterSelector2();
  auto reg_io_selector = RegisterIOSelector(&registers, &selector);
  auto _register = reg_io_selector.get(Instruction(0xff, (uint16_t)0x8BC3));
  ASSERT_EQ(&registers.BX, _register);
}

struct MemoryIOSelectorTests : ::testing::Test {};

TEST_F(MemoryIOSelectorTests, test_get_default_without_sop) {
  auto ram = RAM(128);
  auto bus = BUS::from_device(&ram);
  auto registers = Registers();
  auto mem_io_selector = MemoryIOSelector(&bus, &registers);
  auto _mem =
      (AddressLatch *)mem_io_selector.get(Instruction(0xff, 0x8996, 0xF246));
  ASSERT_NE(_mem, nullptr);
  EXPECT_EQ(0xF246, (uint16_t)_mem->address());
}

TEST_F(MemoryIOSelectorTests, test_get_default_without_sop_2) {
  auto ram = RAM(128);
  auto bus = BUS::from_device(&ram);
  auto registers = Registers();
  registers.BP += 0x22;
  auto mem_io_selector = MemoryIOSelector(&bus, &registers);
  auto _mem =
      (AddressLatch *)mem_io_selector.get(Instruction(0xff, 0x8996, 0xF246));
  ASSERT_NE(_mem, nullptr);
  EXPECT_EQ(0xF246 + 0x22, (uint16_t)_mem->address());
}

#endif // _IO_SELECTOR_TESTS_H_
