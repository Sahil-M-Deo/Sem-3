section .data
    ROWS equ 5
    COLS equ 6

    grid:
        db 0,0,1,0,0,0
        db 0,0,0,0,1,0
        db 0,1,0,0,0,0
        db 0,0,0,0,0,0
        db 1,0,0,1,0,0
    .end:
    
    ;; number of non-bomb cells with 0 neighboring bombs is 3 by default

%if (ROWS*COLS) != (grid.end-grid)
    %error "Size does not match (ROWS, COLS)"
%endif

section .text
    global _start

_start:
;   grid[r*COLS+c] is 1 if that cell has a bomb, 0 otherwise. For every
;   non-bomb cell, count how many of its up-to-8 neighbors (including
;   diagonals) are bombs -- remember cells on an edge or corner have
;   fewer than 8 neighbors. Exit with the number of non-bomb cells whose
;   neighbor-bomb-count is exactly 0.

    mov rax, 60
    syscall
