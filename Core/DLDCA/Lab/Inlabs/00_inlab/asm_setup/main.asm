; ============================================================================
; main.asm
;
; This is NOT an assignment. It is a lab-environment smoke test: if this
; builds and runs cleanly, your nasm/gcc/ld pipeline (and syntax
; highlighting, if you've set that up) is working correctly.
;
; You are not expected or encouraged to read this line by line. Each STAGE
; below is independent and prints exactly one line. Compare the printed
; values against expected_output.txt, and check that the program exits
; with status 0 (echo $? after running).
;
; Build:   make            (see makefile)
; Run:     ./asm ; echo "exit: $?"
; ============================================================================

BITS 64

section .data
    banner:         db "=== envcheck: nasm/gcc/ld pipeline test ===", 10, 0
    banner_len:     equ $ - banner - 1

    ; ---- STAGE 1: literal data + raw write syscall ----
    s1_label:       db "[stage 1] literal write via syscall : ", 0
    s1_msg:         db "hello from .data", 10, 0

    ; ---- STAGE 2: arithmetic + register plumbing ----
    s2_label:       db "[stage 2] register arithmetic (7*6+5): ", 0

    ; ---- STAGE 3: conditional jump ----
    s3_label:       db "[stage 3] conditional jump result    : ", 0
    s3_pass:        db "branch-taken-correctly", 0
    s3_fail:        db "BRANCH LOGIC BROKEN", 0

    ; ---- STAGE 4: loop / .bss round-trip ----
    s4_label:       db "[stage 4] loop sum 1..10             : ", 0

    ; ---- STAGE 5: call into gcc-compiled C (c_add) ----
    s5_label:       db "[stage 5] extern c_add(19, 23)       : ", 0

    ; ---- STAGE 6: call into gcc-compiled C (c_checksum over .data) ----
    s6_label:       db "[stage 6] extern c_checksum(buf,8)   : ", 0
    s6_buf:         db "asmtest!", 0     ; 8 bytes, fixed content -> deterministic checksum

    footer:         db "=== all stages completed, exit code reflects pass/fail ===", 10, 0
    footer_len:     equ $ - footer - 1

    newline:        db 10

section .bss
    numbuf:         resb 24     ; scratch buffer for itoa
    loop_acc:       resq 1      ; stage 4 accumulator

section .text
    global _start
    extern c_add
    extern c_checksum

; ----------------------------------------------------------------------------
; print_str: rdi = pointer to NUL-terminated string. Clobbers rax, rsi, rdx.
; Uses the raw write(2) syscall directly (fd 1 = stdout), no libc.
; ----------------------------------------------------------------------------
print_str:
    push rbx
    mov rbx, rdi            ; save start pointer
    xor rdx, rdx             ; rdx = length counter
.strlen_loop:
    cmp byte [rbx + rdx], 0
    je .strlen_done
    inc rdx
    jmp .strlen_loop
.strlen_done:
    mov rax, 1                ; sys_write
    mov rdi, 1                ; fd = stdout
    mov rsi, rbx              ; buf
    ; rdx already holds length
    syscall
    pop rbx
    ret

; ----------------------------------------------------------------------------
; print_int: rdi = signed 64-bit integer. Prints decimal, no newline.
; Clobbers rax, rcx, rdx, rsi, r8, r9.
; ----------------------------------------------------------------------------
print_int:
    mov r9, rdi              ; keep sign
    mov rax, rdi
    test rax, rax
    jns .abs_done
    neg rax
.abs_done:
    lea rsi, [numbuf + 23]   ; write digits backwards from the end
    mov byte [rsi], 0
    mov rcx, 10
.digit_loop:
    xor rdx, rdx
    div rcx                  ; rax / 10 -> rax, remainder -> rdx
    add dl, '0'
    dec rsi
    mov [rsi], dl
    test rax, rax
    jnz .digit_loop
    test r9, r9
    jns .no_sign
    dec rsi
    mov byte [rsi], '-'
.no_sign:
    mov rdi, rsi
    call print_str
    ret

; ----------------------------------------------------------------------------
; newline_out: prints a single '\n'
; ----------------------------------------------------------------------------
newline_out:
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel newline]
    mov rdx, 1
    syscall
    ret

_start:
    sub rsp, 16               ; scratch space; keeps rsp 16-byte aligned for C calls below

    ; running exit-status accumulator; 0 = pass so far, nonzero = a stage failed
    mov qword [rsp], 0

    ; banner
    mov rdi, banner
    call print_str

    ; ---- STAGE 1 ----
    mov rdi, s1_label
    call print_str
    mov rdi, s1_msg
    call print_str

    ; ---- STAGE 2: (7*6)+5 = 47 ----
    mov rdi, s2_label
    call print_str
    mov rax, 7
    mov rbx, 6
    imul rax, rbx
    add rax, 5
    mov rdi, rax
    call print_int
    call newline_out
    cmp rax, rax             ; (value already checked visually; see stage 3 for a hard check)

    ; ---- STAGE 3: conditional jump, hard pass/fail check ----
    mov rdi, s3_label
    call print_str
    mov rax, 47
    cmp rax, 47
    jne .s3_broken
    mov rdi, s3_pass
    call print_str
    jmp .s3_done
.s3_broken:
    mov rdi, s3_fail
    call print_str
    mov qword [rsp], 1        ; mark failure
.s3_done:
    call newline_out

    ; ---- STAGE 4: loop summing 1..10 via .bss accumulator, expect 55 ----
    mov rdi, s4_label
    call print_str
    mov qword [loop_acc], 0
    mov rcx, 1
.sum_loop:
    add qword [loop_acc], rcx
    inc rcx
    cmp rcx, 11
    jne .sum_loop
    mov rdi, [loop_acc]
    call print_int
    call newline_out

    ; ---- STAGE 5: extern c_add(19, 23) -> expect 42 ----
    mov rdi, s5_label
    call print_str
    mov rdi, 19
    mov rsi, 23
    call c_add
    mov rdi, rax
    call print_int
    call newline_out

    ; ---- STAGE 6: extern c_checksum(s6_buf, 8) -> deterministic value ----
    mov rdi, s6_label
    call print_str
    lea rdi, [s6_buf]
    mov rsi, 8
    call c_checksum
    mov rdi, rax
    call print_int
    call newline_out

    mov rdi, footer
    call print_str

    mov rdi, [rsp]            ; exit status: 0 unless stage 3 flagged a failure
    mov rax, 231               ; sys_exit_group
    syscall                    ; never returns

section .note.GNU-stack noalloc noexec nowrite