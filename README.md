# bfc

A Brainfuck compiler, because why not

# Usage

`bfc [options] <inputfile>`

# Options

`-o <outputfile>` output file

`-S` stop after compilation

`-c` stop after assembling

`--mem-size <size>` number of memory cells, defaults to 30000

`-s|--symbol <sym>` symbol used for the program, defaults to main, see notes

`--no-optimize` disable all optimization

# Notes

Compile with `meson build && ninja -C build`

Rewritten in C. Old C++ version is in old-cxx branch. Some changes:

- Boost dependency is obviously dropped
- Code is callable from C and C++ programs
- Use x86\_64 syscalls instead of interrupt 0x80
- No longer need intermediate files
- Assemble and link with one gcc call

Generated machine code works only on amd64 architecture on linux.

If start symbol is main, program can be run as standalone executable.
Symbols other than main/\_start can be called from C programs by declaring
function `int <symbol>(void)`. The function always returns 0. To call from
a C++ program, declare the prototype inside `extern "C" { }`.
