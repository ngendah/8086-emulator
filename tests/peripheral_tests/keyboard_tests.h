#include "keyboard.h"

#include "gtest/gtest.h"
#include <array>

struct InterruptController : InterruptHandler {
  void interrupt(uint8_t type) override { _type = type; }
  uint8_t _type{};
};

TEST(KeyBoardTest, test_bootstrap) {
  Ports ports;
  InterruptController handler;
  KeyBoard keyboard;
  reinterpret_cast<Device *>(&keyboard)->bootstrap(&ports, &handler);
  EXPECT_EQ(ports._ports.size(), 1);
  uint8_t key = 0xfa;
  keyboard.process_input(&key);
  EXPECT_EQ(handler._type, 0x21);
}
