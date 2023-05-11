cmake_minimum_required(VERSION 3.10)
add_executable(unitTests tests.cpp)

include_directories(
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src/common"
        "${PROJECT_SOURCE_DIR}/src/cpu"
        "${PROJECT_SOURCE_DIR}/src/peripheral"
        ${PROJECT_SOURCE_DIR}/libraries/googletest/googletest/include
)

target_include_directories(unitTests PUBLIC "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/src/common"
        "${PROJECT_SOURCE_DIR}/src/cpu"
        "${PROJECT_SOURCE_DIR}/src/peripheral")

add_subdirectory(peripheral_tests)
add_subdirectory(instruction_set_tests)
add_subdirectory(execution_unit_tests)

target_link_libraries(
  unitTests
  gtest_main
  pthread
  plog
  fmt
  peripheral
 cpu 
  common
)

add_test(runUnitTests unitTests)
