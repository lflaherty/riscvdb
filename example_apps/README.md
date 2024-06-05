# RV32I Example Programs

The small example programs in this directory demonstrate usage of the RV32I emulated CPU.

## Examples

* `fib` Calculates the nth value of the fibonacci sequence. Demonastrates building and loading a binary with no standard library. The linker is explicitly invoked, and includes minimal symbols beyond the main method. The startup assembly defines a `_start` entrypoint and `_exit` point to add a breakpoint to. No libc usage is possible in this program.
* `random` Calculates an array of random values independently of the standard library. This program uses the `riscv64-unknown-elf` toolchain to define the entry point and linking is not called explicitly like in `fib`.
* `quicksort` Similar to `random`, but sorts the random array using an internally defined quicksort algorithm.
* `quicksort_stl` This generates a random array and uses the quicksort algorithm to sort it, however this example invokes the libc to help us with this, and is the most complete example of using the standard library.
* `strings` More examples of libc usage via `snprintf` and using the string print functionality. Note that the sim does not currently support any outputs that will allow `printf`.