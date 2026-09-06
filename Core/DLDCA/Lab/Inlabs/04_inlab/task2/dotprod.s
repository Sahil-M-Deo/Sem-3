section .data
    vec1:
        dq 5, -2, 8, 3, -1, 6
    .end:

    vec2:
        dq 2, 6, -3, 4, 7, 5
    .end:

    vecsize equ (vec1.end - vec1)/8

    ;; dot(vec1, vec2) is 9 by default
    
%if (vec1.end-vec1) != (vec2.end-vec2)
    %error "Both vectors should have same size"
%endif

section .text
    global _start

_start:
;   Find the dot product of vec1 and vec2 and put it in `rdi`    

    mov rax, 60
    syscall