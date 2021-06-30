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
xor esi, esi

jmp main ; launch program

; just to be absolutely sure it halts when it gets here
hlt
jmp $
jmp $
