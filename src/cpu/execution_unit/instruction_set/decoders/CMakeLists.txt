add_library(decoders decoders.cpp value_io.cpp)

target_link_libraries(decoders PUBLIC core)

