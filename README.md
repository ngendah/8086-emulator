# 8086 Emulator

![tests](https://github.com/ngendah/8086-emulator/actions/workflows/linux.yml/badge.svg)

======================

### Dependencies:
1. clang or gcc, cmake, clang-tools, clang-tidy, ninja

2. SDL2

3. git submodules: fmt, plog and googletest

### Build

```
 cmake . -G Ninja
 cmake --build . # --verbose
 ctest # gdb tests/unitTests
```
