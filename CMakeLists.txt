cmake_minimum_required(VERSION 3.10)
project(8086)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
find_package(SDL2 REQUIRED)
add_compile_options(-Wall -Wextra -pedantic -Werror)
include_directories(
        include
        libraries/plog/include
        libraries/fmt/include
        ${SDL2_INCLUDE_DIRS}
)
add_subdirectory(src)
add_subdirectory(libraries)
enable_testing()
add_subdirectory(tests)
