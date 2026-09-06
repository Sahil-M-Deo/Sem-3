#include "translocation.h"

// Reverses v[L..R] in place, using only O(1) extra memory.
void reverse_segment(vector<char> &v, int L, int R) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    while (L < R) {
        char temp = v[L];
        v[L] = v[R];
        v[R] = temp;
        L++;
        R--;
    }
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

void apply_translocation(vector<char> &v, int L, int R, int P) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    if (P > R) {
        // Window v[L..P-1] holds [segment][gap]; make it [gap][segment].
        reverse_segment(v, L, R);
        reverse_segment(v, R + 1, P - 1);
        reverse_segment(v, L, P - 1);
    } else if (P < L) {
        // Window v[P..R] holds [gap][segment]; make it [segment][gap].
        reverse_segment(v, P, L - 1);
        reverse_segment(v, L, R);
        reverse_segment(v, P, R);
    }
    // P == L or P == R + 1 leaves the sequence unchanged.
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
