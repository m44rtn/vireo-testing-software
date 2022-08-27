# Setup

A very basic program that is capable of installing Vireo.  

__!! WARNING !!__ Do NOT use this software on real hardware. It will overwrite data without user confirmation! This will change in the  near future with the introduction of loadable system drivers.

Currently, this setup program will install Vireo on disk HD0P0 (harddisk 0, partition 0). In the future some user interaction will be added where a user can select a disk. Unfortunately, due to the complexity of modern bootloaders, this setup program is unable
to install third-party bootloaders like GRUB or SYSLINUX. This in turn means that the setup program is not actually used to install Vireo, this will be done using a DOS system. However, the program
and Vireo are capable of copying their own files from a CD/DVD onto a harddisk and writing a boot sector image to the first sector of a partition. Therefore, this still completes 
the goals set for kernel-v0.1 of the Vireo-II kernel.

## Prerequisites for building
Vireo syslib header files should be installed into ./lib/*.

- `make`
- a cross `GCC (i686-elf)` and `binutils (i686-elf)`
- `nasm`

## Building the software
- run `make`

