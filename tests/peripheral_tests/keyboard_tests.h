#include "keyboard.h"

#include "gtest/gtest.h"
#include <array>

struct KeyboardT : KeyBoard {
  void bootstrap(Ports *ports, InterruptHandler *handler) override {
    KeyBoard::bootstrap(ports, handler);
  }
};

struct InterruptController : InterruptHandler {
  void interrupt(uint8_t type) override { _type = type; }
  uint8_t _type{};
};

TEST(KeyBoardTest, test_bootstrap) {
  Ports ports;
  InterruptController handler;
  KeyboardT keyboard;
  keyboard.bootstrap(&ports, &handler);
  EXPECT_EQ(ports.size(), 1);
  uint8_t key = 'q';
  keyboard.process_input(&key);
  EXPECT_EQ(handler._type, key);
}
