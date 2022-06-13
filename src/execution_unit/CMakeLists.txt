add_library(execution_unit cpu.cpp decoder_impl.cpp)

add_subdirectory(instruction_set)

include_directories(instruction_set)

target_link_libraries(execution_unit PUBLIC instruction_set)
target_include_directories(execution_unit PUBLIC instruction_set)
