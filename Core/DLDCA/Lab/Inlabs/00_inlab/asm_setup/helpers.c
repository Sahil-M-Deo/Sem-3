// helpers.c
// Tiny C-side helpers, called from envcheck.asm to verify the assembler
// can correctly call into gcc-compiled code (System V AMD64 calling
// convention: integer args in rdi, rsi, rdx, ...; return value in rax).
//
// Kept deliberately trivial -- this file exists to prove the toolchain
// links ASM + C correctly, not to contain anything worth reading.

long c_add(long a, long b) {
    return a + b;
}

long c_checksum(const unsigned char *buf, long len) {
    long sum = 0;
    for (long i = 0; i < len; i++) {
        sum = (sum * 31) + buf[i];
    }
    return sum;
}