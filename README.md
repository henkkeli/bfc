# bfc

A Brainfuck compiler, because why not

# usage

`bfc [options] <inputfile>`

# options

`-o <outputfile>` output file

`-S` stop after compilation

`-c` stop after assembling

`--mem-size <size>` number of memory cells, defaults to 30000

# dependencies

[Boost](http://www.boost.org/)

# other stuff

Generated machine code works only on amd64 architecture on linux.
