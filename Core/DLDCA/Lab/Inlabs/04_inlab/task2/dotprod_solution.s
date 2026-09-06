section .data
    vec1:
        dq 5, -2, 8, 3, -1, 6
    .end:

    vec2:
        dq 2, 6, -3, 4, 7, 5
    .end:

    vecsize equ (vec1.end - vec1)/8

    ;; dotprod(vec1, vec2) is 9 by default
    
%if (vec1.end-vec1) != (vec2.end-vec2)
    %error "Both vectors should have same size"
%endif

section .text
    global _start

_start:
;   Find the dot product of vec1 and vec2 and put it in `rdi`    
    xor edi, edi
    xor ecx, ecx

// ! note this was wrongly done in SL3 where I put the `cmp` check at the bottom.. why is that wrong?
.for:
    cmp rcx, vecsize
    jge .done

    mov rsi, [vec1 + 8*rcx]
    imul rsi, [vec2 + 8*rcx]
    add rdi, rsi

    inc rcx
    jmp .for

.done:
    mov rax, 60
    syscall