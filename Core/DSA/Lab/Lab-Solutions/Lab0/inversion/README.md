# Lab 01 - Chromosomal Inversion

[Ctrl + Shift + V to see the rendered version of this file]

## Background

DNA is a long sequence of bases, written with the letters `A`, `C`, `G` and `T`.
Sometimes a segment of a chromosome breaks off, flips around, and reattaches at the
same place, so the bases in that stretch end up in the opposite order. Biologists call
this an inversion.

You are writing the firmware for a bio-sensor that simulates this process. The sensor
is a tiny embedded device. It has just enough memory to hold the sequence itself, and
almost nothing left over for scratch space.

## Problem Statement

You are given a DNA sequence of length `N`, stored as a `vector<char>` named `v`, and
two 0-indexed integers `L` and `R`.

Reverse the segment `v[L..R]`, both ends included, in place. Everything outside that
segment must stay exactly where it is.

Reading the input, looping over the test cases and printing the answer are already done
for you in `main.cpp`. All you have to write is `reverse_segment`.

## Your Task

Write your code in `inversion.cpp`, inside the body of `reverse_segment`:

```cpp
void reverse_segment(vector<char> &v, int L, int R) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    ...
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
```

Do not delete or move the two marker comments, do not write anything outside them, and
do not change any other file. Everything this problem needs fits inside the function
body, so you do not need helper functions here.

## Directory Structure

```
inversion.cpp    # write your solution here
inversion.h      # declaration of reverse_segment (DO NOT MODIFY)
common.h         # the headers you are allowed to use (DO NOT MODIFY)
main.cpp         # reads input, calls your function, prints the result (DO NOT MODIFY)
README.md        # this file (DO NOT MODIFY)
Makefile         # build automation
```

## Rules of the Problem

### Function Specification

#### `void reverse_segment(vector<char> &v, int L, int R)`

Declared in `inversion.h`, called by `main.cpp`, written by you in `inversion.cpp`.

It reverses the elements of `v` from index `L` to index `R`, inclusive, modifying `v`
in place.

Parameters:

- `v` - the DNA sequence, modified in place
- `L`, `R` - first and last index of the segment to reverse, `0 <= L <= R <= N - 1`

A segment of length 1 (`L == R`) is already reversed, so `v` must come out unchanged.

The function returns nothing. `main.cpp` prints `v` after calling it, so a version that
works out the right answer without storing it back into `v` earns no marks.

### Constraints

| Resource | Limit |
|---|---|
| Time | `O(N)` per test case |
| Auxiliary space | `O(1)` |

- Building a second `vector` (or array, or `string`) that holds the reversed segment
  and copying it back uses `O(N)` extra memory and is not allowed. The only large
  object in memory must be `v` itself.
- `reverse_segment` may use only a constant number of extra variables, say a couple of
  indices and a single `char` to swap through.
- Move the bases around by writing to `v[i]` directly. Do not use library helpers such
  as `std::reverse` or `std::swap`, and do not include any extra headers. Everything
  you are allowed to use is already included by `common.h`.

### Input and Output

Input format:

```
Q
N_1
sequence_1
L_1 R_1
N_2
sequence_2
L_2 R_2
...
```

- `Q` is the number of independent test cases
- For each test case: the length `N`, then the `N` bases on their own line, then the two
  integers `L` and `R` on the next line

The bases are read one at a time into a `vector<char>` of size `N`; that part is
already written for you.

Output format: one line per test case, holding the `N` bases with no separators after
the inversion.

Example input:

```
2
10
AACGTTGGCA
2 6
6
ACGTAC
0 5
```

Example output:

```
AAGTTGCGCA
CATGCA
```

Walkthrough of the first query (`L = 2`, `R = 6`):

```
index:    0 1 2 3 4 5 6 7 8 9
v:        A A C G T T G G C A
segment:      ^^^^^^^^^ (v[2..6] = C G T T G)
reversed:     G T T G C

result:   A A G T T G C G C A
          (indices 0, 1, 7, 8, 9 are untouched)
```

The second query reverses the whole sequence, since `L = 0` and `R = N - 1`.

### Assumptions

- `1 <= N <= 10^7`
- `0 <= L <= R <= N - 1`
- every element of `v` is one of the bases `A`, `C`, `G` and `T`

### Hint (only if you are stuck)

The first base of the segment has to end up where the last one is, the second where the
second-last one is, and so on. That is a swap you can do straight away, with nothing to
store anywhere. Then step both ends inward and repeat. When do you stop?

## Bonus

Once this works, have a look at `translocation/README.md`. There you have to move a
segment to a different place in the sequence instead of flipping it, and the
`reverse_segment` you just wrote is the only tool you need for it.

---
