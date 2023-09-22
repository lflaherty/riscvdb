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

To display the help page, simply run `riscvdb -h`. To display a list of commands supported from within riscdb, run `help` from the prompt.

### Executing a binary

Load the binary with either `riscvdb file.bin` or the `load` command from the prompt.

riscvdb currently only supports raw binary files.

Once the binary has loaded, invoke `run` or `r`. Execution can be interrupted using Ctrl-C.

### Debugging

The following debugging commands are supported.

| Command | Short form | Description | Usage |
| ------- | ---------- | ----------- | ----- |
| `help` | `h` | Display this help from within riscvdb | `help` |
| `load` | `l` | Load a new binary | `load <filename>` |
| `run` | `r` | Start execution from the beginning | `r` |
| `continue` | `c` | Continue execution from current PC | `c` |
| `break` | `b` | Set a breakpoint | `b <instruction addr>` |
| `delete` | `d` | Delete a breakpoint | `d <breakpoint number>` |
| `print` | `p` | Print the value of a register | `b <register name>` |
| `next` | `n` | Run the next instruction and break again | `n` |
| `quit` `exit` | `q` | Exit riscvdb | `q` |
