cmake_minimum_required(VERSION 3.10)
project(8086)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -pedantic -Werror)
include_directories(
  includes
  libraries/plog/include
  libraries/fmt/include
)
add_subdirectory(src)
add_subdirectory(libraries)
enable_testing()
add_subdirectory(tests)
