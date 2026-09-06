#pragma once

#include "common.h"

// Reverses the elements of v from index L to index R (both ends inclusive),
// in place. Everything outside v[L..R] stays exactly where it is.
// A segment of length 1 (L == R) leaves v unchanged.
void reverse_segment(vector<char> &v, int L, int R);
