# Breaker

This program is used to test most system calls of the Vireo-II kernel. At some point in its life, it may crash/break the kernel, thus the name. It uses the vireo-syslib from [this repository](https://github.com/m44rtn/vireo).

## Prerequisites for building
- `make`
- a cross `GCC (i686-elf)` and `binutils (i686-elf)`
- `nasm`

## Building the software
- run `make`

## Screenshot
From commit 43fde84:

![screenshot of Breaker](https://gyazo.com/6d6ce8ffb8cd9b97bc166ab003c086de.png)

