add_library(
        instructions
        STATIC
        src/mov.cpp
        src/push.cpp
        src/pop.cpp
        src/xchg.cpp
        src/in.cpp
        src/out.cpp
        src/xlat.cpp
        src/lea.cpp
        src/lpa.cpp
        src/les.cpp
        src/lds.cpp
        src/incr.cpp
        src/cmp.cpp
        src/lahf.cpp
        src/sahf.cpp
        src/conditional_jmp.cpp
        src/dec.cpp
        src/noop.cpp
)

target_include_directories(instructions PUBLIC ../core/include include)
