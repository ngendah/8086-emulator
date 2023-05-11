add_library(instruction_set_core STATIC src/decoders.cpp src/value_io.cpp)

target_include_directories(instruction_set_core PUBLIC include)
