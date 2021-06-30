# Conway

Conway simulates Conway's Game of Life and is used to test the loading of ELF binaries by the Vireo kernel. In the future, it will also be used to test the System API calls of Vireo.

The software uses a small subset of kernel source files which are modified to work as a program, since there are no API calls currently. It also expects to run in the supervisor mode (ring 0) of the CPU.

## Prerequisites for building
To build this software you need the following software:
* `GCC` built as a cross compiler for the i686 architecture
* `NASM`
* `GNU make`

**NOTE:** The GCC cross compiler should be included in the `PATH` variable of your shell

## Building the software
* To build the software run `make all`
* To generate a map file showing the mapping of the binary run `make map`, this will generate a file called `kernel.map`