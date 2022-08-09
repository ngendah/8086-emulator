include_directories(include)

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
  les.cpp
)
