add_library(
  instruction_set
  mov.cpp
  push.cpp
  pop.cpp
  xchg.cpp
  instruction_set.cpp
  in.cpp
  out.cpp
  xlat.cpp
  lea.cpp
  lpa.cpp
  les.cpp
  lds.cpp
  incr.cpp
  cmp.cpp
  lahf.cpp
  sahf.cpp
  conditional_jmp.cpp
  dec.cpp
  noop.cpp
)

add_subdirectory(decoders)
add_subdirectory(include)

include_directories(include decoders)

target_link_libraries(instruction_set PUBLIC core decoders)
target_include_directories(instruction_set PUBLIC decoders include)
