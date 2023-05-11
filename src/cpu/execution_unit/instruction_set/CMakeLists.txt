add_library(
  instruction_set
        STATIC
  instruction_set.cpp
)

add_subdirectory(core)
add_subdirectory(instructions)

target_include_directories(instruction_set PUBLIC core/include instructions/include)
target_link_libraries(instruction_set PUBLIC core common instruction_set_core instructions)
