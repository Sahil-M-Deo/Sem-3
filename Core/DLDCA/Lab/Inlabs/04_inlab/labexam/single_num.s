section .data
    array: 
        dd 3, 6, 1, 42, 8, 1, 3, 6, 8
    .end:
    array_len equ (array.end - array)/4

section .text
    global _start

_start:



    mov eax, 60
    syscall
    