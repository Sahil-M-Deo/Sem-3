#pragma once
#include "common.h"

// Area of the largest rectangle that can be cut out of a patchwork of strips
// of lengths `heights`, each 1 wide. Both functions return the same number.

// O(n^2): walks out from every strip.
long long largestRectangleBruteForce(const vector<int> &heights);

// O(n).
long long largestRectangleOptimal(const vector<int> &heights);
