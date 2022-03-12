bits 32

global start
extern main

section .text
start:
pushad
xor eax, eax
xor edi, edi
xor ebx, ebx
xor edx, edx
xor ecx, ecx
xor esi, esi

call main ; launch program

popad
ret
