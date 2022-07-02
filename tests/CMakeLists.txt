cmake_minimum_required(VERSION 3.10)

add_executable(unitTests tests.cpp)

include_directories(
  "${PROJECT_SOURCE_DIR}/includes"
  "${PROJECT_SOURCE_DIR}/src/peripheral"
  "${PROJECT_SOURCE_DIR}/src/execution_unit"
  "${PROJECT_SOURCE_DIR}/src/execution_unit/instruction_set"
  "${PROJECT_SOURCE_DIR}/src/execution_unit/instruction_set/include"
)

include_directories(
  ${PROJECT_SOURCE_DIR}/libraries/googletest/googletest/include
)

add_subdirectory(peripheral_tests)
add_subdirectory(instruction_set_tests)

target_link_libraries(
  unitTests
  gtest_main
  pthread
  plog
  fmt
  peripheral
  execution_unit
)

add_test(runUnitTests unitTests)
