bits 32

global start
extern main

section .text
start:

call main ; launch program
; Do NOT change eax after this!

ret
