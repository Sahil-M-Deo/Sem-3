section .data
    a: dq 10
    b: dq 20

section .text
    global _start

_start:
;   move `a` into `rdi` and add `b` to it
    mov rdi, qword [a]
    add rdi, qword [b]

    mov rax, 60
    syscall