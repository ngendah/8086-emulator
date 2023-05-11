add_library(cpu STATIC cpu.cpp)

add_subdirectory(core)
add_subdirectory(execution_unit)

target_include_directories(cpu PUBLIC core execution_unit)
target_link_libraries(cpu PUBLIC execution_unit)
