add_library(instruction_unit_core
  STATIC
  src/bus_io.cpp
  src/decoders.cpp
  src/value_io.cpp
  )

target_include_directories(instruction_unit_core PUBLIC include)
