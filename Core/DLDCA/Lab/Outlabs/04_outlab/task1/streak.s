section .data
    values:
        dq 4, 4, 4, 7, 7, 2, 2, 2, 2, 9, 9, 1, 7, 7, 7
    .end:
    n equ (values.end - values)/8

    ;; longest run of consecutive equal values is 4 by default

section .text
    global _start

_start:
;   find the length of the longest run of consecutive equal values in
;   `values`, and exit with that length

    mov rax, 60
    syscall
