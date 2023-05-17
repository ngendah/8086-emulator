add_library(
  devices
  STATIC
  src/ram.cpp
  src/display.cpp
  src/keyboard.cpp
  src/pointer.cpp
)

target_include_directories(devices PUBLIC include)
