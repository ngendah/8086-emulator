#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

#include "execution_unit_tests/execution_unit_tests.h"
#include "instruction_set_tests/cmp_tests.h"
#include "instruction_set_tests/in_tests.h"
#include "instruction_set_tests/incr_tests.h"
#include "instruction_set_tests/instruction_set_tests.h"
#include "instruction_set_tests/io_selectors_tests.h"
#include "instruction_set_tests/lahf_tests.h"
#include "instruction_set_tests/lds_tests.h"
#include "instruction_set_tests/lea_tests.h"
#include "instruction_set_tests/les_tests.h"
#include "instruction_set_tests/mov_tests.h"
#include "instruction_set_tests/out_tests.h"
#include "instruction_set_tests/pop_tests.h"
#include "instruction_set_tests/push_tests.h"
#include "instruction_set_tests/sahf_tests.h"
#include "instruction_set_tests/segment_mapper_tests.h"
#include "instruction_set_tests/xchg_tests.h"
#include "instruction_set_tests/xlat_tests.h"
#include "peripheral_tests/ram_tests.h"
#include "types_tests.h"

int main(int argc, char **argv) {
  static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::verbose, &consoleAppender);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
