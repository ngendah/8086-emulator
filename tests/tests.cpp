#ifndef _TESTS_H_
#define _TESTS_H_

#include "execution_unit_tests/execution_unit_tests.h"
#include "execution_unit_tests/execution_unit_tests_1.h"
#include "instruction_unit_tests/bus_address_latch_tests.h"
#include "instruction_unit_tests/cmp_tests.h"
#include "instruction_unit_tests/in_tests.h"
#include "instruction_unit_tests/incr_tests.h"
#include "instruction_unit_tests/instruction_unit_tests.h"
#include "instruction_unit_tests/int_tests.h"
#include "instruction_unit_tests/io_selectors_tests.h"
#include "instruction_unit_tests/iret_tests.h"
#include "instruction_unit_tests/lahf_tests.h"
#include "instruction_unit_tests/lds_tests.h"
#include "instruction_unit_tests/lea_tests.h"
#include "instruction_unit_tests/les_tests.h"
#include "instruction_unit_tests/mov_tests.h"
#include "instruction_unit_tests/out_tests.h"
#include "instruction_unit_tests/pop_tests.h"
#include "instruction_unit_tests/push_tests.h"
#include "instruction_unit_tests/sahf_tests.h"
#include "instruction_unit_tests/segment_mapper_tests.h"
#include "instruction_unit_tests/xchg_tests.h"
#include "instruction_unit_tests/xlat_tests.h"
#include "peripheral_tests/keyboard_tests.h"
#include "peripheral_tests/ram_tests.h"
#include "types_tests.h"

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

int main(int argc, char **argv) {
  static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::verbose, &consoleAppender);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#endif // _TESTS_H_
