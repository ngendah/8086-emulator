add_library(
  instruction_unit
  STATIC
  instruction_unit.cpp
)

add_subdirectory(core)
add_subdirectory(instructions)

target_include_directories(instruction_unit PUBLIC core/include instructions/include)
target_link_libraries(instruction_unit PUBLIC core common instruction_unit_core instructions)
