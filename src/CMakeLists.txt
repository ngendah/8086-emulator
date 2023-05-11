cmake_minimum_required(VERSION 3.10)

add_executable(8086 8086.cpp)

add_subdirectory(common)
add_subdirectory(cpu)
add_subdirectory(peripheral)

target_include_directories(8086 PRIVATE cpu peripheral)

target_link_libraries(
  8086
        PUBLIC
  common
  cpu
  peripheral
  fmt
  plog
  )

target_link_libraries(
        8086
        PRIVATE
        ${SDL2_LIBRARIES}
)