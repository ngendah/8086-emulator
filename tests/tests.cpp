#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include "instruction_set_tests/segment_mapper_tests.h"
#include "peripheral_tests/ram_tests.h"
#include "types_tests.h"

int main(int argc, char **argv) {
  static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::verbose, &consoleAppender);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
