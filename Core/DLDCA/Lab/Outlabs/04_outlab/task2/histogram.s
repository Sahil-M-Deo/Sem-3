section .data
    text:
        db "mississippi"
    .end:
    text_len equ (text.end - text)

    counts: times 26 dd 0

    ;; the most common letter's count is 4 by default

section .text
    global _start

_start:
;   build a histogram of letter counts into `counts` (counts[c - 'a']
;   for each byte c in `text`), then exit with the highest count found
;   in `counts`

    mov rax, 60
    syscall
