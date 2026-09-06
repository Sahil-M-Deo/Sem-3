section .data
    array: 
        dd 3, 6, 1, 42, 8, 1, 3, 6, 8
    .end:
    array_len equ (array.end - array)/4

section .text
    global _start

_start:
    xor edi, edi

    xor ecx, ecx
.loopbegin:
    cmp rcx, array_len
    jge .loopend

    xor edi, dword [array+4*rcx]

    inc rcx
    jmp .loopbegin
.loopend:

    mov eax, 60
    syscall
    