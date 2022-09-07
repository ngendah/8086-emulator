add_subdirectory(decoders)

include_directories(include decoders)

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
)

target_link_libraries(instruction_set PUBLIC decoders)
target_include_directories(instruction_set PUBLIC decoders)
