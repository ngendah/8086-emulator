add_library(decoders OBJECT decoders.cpp value_io.cpp)

target_link_libraries(decoders PUBLIC core)

