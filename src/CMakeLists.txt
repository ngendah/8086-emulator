cmake_minimum_required(VERSION 3.10)

add_executable(8086 8086.cpp)

add_subdirectory(peripheral)
add_subdirectory(execution_unit)

include_directories(
  peripheral
  execution_unit
  )

target_include_directories(8086 PUBLIC peripheral)
target_include_directories(8086 PUBLIC execution_unit)

target_link_libraries(
  8086
  plog 
  fmt 
  peripheral 
  execution_unit
)

