# riscvdb
RISC V simulator and debugger

Runs RISC V binaries on a simluated single core processor. Capable of gdb-like debugging.

## Building

First, ensure the submodules are present (if not originally cloned using `--recurse-submodules`):

```
git submodule init
git submodule update
```

Then use CMake to build:

```
mkdir build
cd build
cmake ..
```

This build the binary as `build/riscvdb`.

## Running


