#pragma once

#include "common.h"

// Reverses the elements of v from index L to index R (both ends inclusive),
// in place. Exactly the function from the inversion problem, unchanged.
// If L >= R there is nothing to do.
void reverse_segment(vector<char> &v, int L, int R);

// Cuts the segment v[L..R] out of v and re-inserts it immediately before the
// base that was originally at index P, in place. P never lies strictly inside
// the segment; P == L and P == R + 1 leave v unchanged.
void apply_translocation(vector<char> &v, int L, int R, int P);
