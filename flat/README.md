# Flat

Flat is a short program that generates a divide by zero exception within the CPU, and thus a kernel panic within Vireo, to demonstrate it was loaded successfully by the kernel. It does so by executing interrupt #0. 

## Prerequisites for building
To build this software, you need NASM. 

## Building the software
You can use the following command to build the software:

```SHELL
nasm test.asm -f bin -o test.bin
```