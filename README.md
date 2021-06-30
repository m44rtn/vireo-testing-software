# Testing software for the Vireo kernel
This repository contains software that is used to test the Vireo-II kernel. The Vireo-II kernel is a mono-tasking kernel for the x86 CPU architecture. For more information, please refer to the [Vireo kernel repository](https://github.com/m44rtn/vireo-kernel).

## Goal
The goal of this repository is to organise and store the source code of all the software that is used to test and develop the Vireo kernel. The software will be updated as development on Vireo progresses.

## Software in this repository
The repository currently contains the following software:

* `flat`, the source code of an assembly file that generates a divide by zero exception on purpose by executing `int 0`;
* `conway`, a C program that simulates Conway's Game of Life, it is compiled into an ELF binary named `conway.elf`.

## Building and more information
For more information, and information on how to build the software, please refer to the README included in the folders of the respective software.
